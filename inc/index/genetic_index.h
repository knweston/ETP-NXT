#ifndef GENETIC_INDEX_H
#define GENETIC_INDEX_H

#include "index/index.h"

struct Gene {
    uint32_t m_a;
    uint32_t m_b;
    Gene() {}
    Gene(uint32_t x_a, uint32_t x_b)
        :m_a(x_a), m_b(x_b) {}

    Gene(const Gene& x_gene) {
        m_a = x_gene.m_a;
        m_b = x_gene.m_b;
    }
    string to_string() {
        string str = std::to_string(m_a) + ", " + std::to_string(m_b);
        return str;
    }

    friend bool operator== (const Gene &g1, const Gene &g2) { return ((g1.m_a == g2.m_a) && (g1.m_b == g2.m_b)); }
    friend bool operator!= (const Gene &g1, const Gene &g2) { return ((g1.m_a != g2.m_a) || (g1.m_b != g2.m_b)); }
};

class Candidate {
public:
    Candidate() {
        m_score = 0;
        m_sequence = vector<bool>(MAX_ADDRESS_BITS, 0);
    }

    Candidate(vector<uint32_t> x_sequence) {
        m_score = 0;
        m_sequence = vector<bool>(MAX_ADDRESS_BITS, 0);
        initialize(x_sequence);
    }

    ~Candidate() {}
    void fitness(double x_score) { m_score = x_score; }
    void genes(vector<Gene> x_genes) { m_genes = x_genes; }
    double fitness() { return m_score; }
    vector<Gene> genes() { return m_genes; }
    void update_sequence(uint32_t x_pos, bool x_val) { m_sequence[x_pos] = x_val; }
    bool is_in_sequence(uint32_t x_bit) { return m_sequence[x_bit]; }

    uint32_t modify_gene(uint32_t x_pos, uint32_t x_val, uint32_t x_segment) {
        uint32_t old_val = MAX_ADDRESS_BITS;
        if (x_segment == 0) { // segment B
            old_val = m_genes[x_pos].m_b;
            m_genes[x_pos].m_b = x_val;
        }
        else {               // segment A
            old_val = m_genes[x_pos].m_a;
            m_genes[x_pos].m_a = x_val;
        }
        return old_val;
    }

    Gene replace_gene(uint32_t x_pos, Gene x_gene) {
        Gene replaced_gene = m_genes[x_pos];
        m_genes[x_pos] = x_gene;
        return replaced_gene;
    }

    void initialize(vector<uint32_t>& x_sequence) {
        for (uint32_t i=0; i < x_sequence.size(); i+=2) {
            Gene new_gene = Gene(x_sequence[i+1], x_sequence[i]);
            m_genes.push_back(new_gene);
        }
        for (uint32_t i=0; i < x_sequence.size(); ++i)
            m_sequence[x_sequence[i]] = true;
    }

    string to_string() {
        string str = "";
        for (int32_t i=m_genes.size()-1; i >= 0; --i) {
            str += m_genes[i].to_string() + ", ";
        }
        str.pop_back();
        str.pop_back();
        return str;
    }

    void print_sequence() {
        for (uint32_t i=0; i < m_sequence.size(); ++i)
            cout << m_sequence[i] << " ";
        cout << endl;
    }

    Candidate(const Candidate& x_candidate) {
        m_genes = x_candidate.m_genes;
        m_sequence = x_candidate.m_sequence;
        m_score = x_candidate.m_score;
    }

    friend bool operator<  (const Candidate &c1, const Candidate &c2) { return c1.m_score <  c2.m_score; }
    friend bool operator<= (const Candidate &c1, const Candidate &c2) { return c1.m_score <= c2.m_score; }
    friend bool operator>  (const Candidate &c1, const Candidate &c2) { return c1.m_score >  c2.m_score; }
    friend bool operator>= (const Candidate &c1, const Candidate &c2) { return c1.m_score >= c2.m_score; }
    friend bool operator== (const Candidate &c1, const Candidate &c2) {
        if (c1.m_genes.size() != c2.m_genes.size())
            return false;
        return c1.m_genes == c2.m_genes;
    }

protected:
    vector<Gene> m_genes;
    vector<bool> m_sequence; // sequence of selected bits from all genes 
    double m_score; // fitness score
};



//========================================================================================================================//
// GENETIC INDEX
//========================================================================================================================//
class GeneticIndex : public Index {
public:
    GeneticIndex(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length,
                 uint32_t remap_length, uint32_t x_fill_lvl, BLOCK **arr, uint32_t cpu)
        :Index(x_numset, x_numway, x_interval_length), 
         m_num_warmup_interval(2),
         m_evaluation_done(false)
    {
        REMAP_INTERVAL = remap_length;
        FILL_LEVEL     = x_fill_lvl;
        block_array    = arr;
        CPU_ID         = cpu;

        // multi-core support
        cout << "CONFIG FILE: " << CONFIG_FILE << endl;
        for (uint32_t i=0; i<NUM_CPUS; ++i) {
            cout << "APP_LIST[" << i << "]: " << APP_LIST[i] << endl;
            vector<uint32_t> dna_sequence = read_dna_config(APP_LIST[i]);
            Candidate new_candidate = Candidate(dna_sequence);
            m_population.push_back(new_candidate);
            cout << "[Genetic Algorithm] candidate: " << m_population[i].to_string() << endl;
        }

        // if only 1 candidate <=> single-program mode
        if (m_population.size() == 1) {
            m_evaluation_done = true;
            current_candidate_idx = 0;
            last_candidate_idx = 0;
        }
        else {
            // if there is a warmup phase => use nohash idx during the warmup intervals
            if (m_num_warmup_interval > 0) {
                current_candidate_idx = m_population.size();
                last_candidate_idx = m_population.size();
            }
            else {
                current_candidate_idx = 0;
                last_candidate_idx = 0;
            }
        }   
        
        m_scoreboard = vector<uint32_t>(m_population.size(), 0);
        
        G_ACCESS    = new uint64_t [NUM_SET];
        PI_G_ACCESS = new uint64_t [NUM_SET];
        memset(G_ACCESS, 0, NUM_SET * sizeof(*G_ACCESS));
        memset(PI_G_ACCESS, 0, NUM_SET * sizeof(*PI_G_ACCESS));
    }

    ~GeneticIndex() {}
    uint32_t get_set(uint64_t address);
    void     print_stats();
    void     update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type);
    void     move_cache_data(MEMORY *lower_level);
    uint32_t compute_index(uint64_t address, uint32_t candidate_idx);

    vector<uint32_t> read_dna_config(string app_name);
    static vector<uint32_t> parse_config(string input_line);

    // replacement policy
    uint32_t get_drrip_victim(uint32_t set);
    void     drrip_update(uint32_t set, uint32_t way, uint32_t src_age);
    uint32_t get_lru_victim(uint32_t set);
    void     lru_update(uint32_t set, uint32_t way);
    
protected:
    vector<Candidate> m_population;
    uint32_t current_candidate_idx;
    uint32_t last_candidate_idx;

    uint64_t* G_ACCESS;     
    uint64_t* PI_G_ACCESS;  

    uint32_t m_num_warmup_interval;
    bool m_evaluation_done;
    vector<uint32_t> m_scoreboard;

    uint32_t REMAP_INTERVAL;
    uint32_t FILL_LEVEL;
    uint32_t CPU_ID;
    BLOCK**  block_array; // ptr to the cache set data array

    uint32_t map_ptr       = 0;
    uint64_t num_writeback = 0;
    uint64_t num_drop      = 0;
    uint64_t num_read      = 0;
};

#endif // GENETIC_INDEX_H