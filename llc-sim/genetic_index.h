#ifndef GENETIC_INDEX_H
#define GENETIC_INDEX_H

#include "index.h"

#define NUM_GENERATIONS 100
#define TOP_CANDIDATE_CUTOFF 15
#define POPULATION_AFTER_EMIGRATION TOP_CANDIDATE_CUTOFF
#define NUM_INIT_CANDIDATES 250
#define INIT_EMIGRATION_FREQ 0  // 0 = turn off emigration feature
#define INIT_MUTATION_RATE 0.75
#define MAX_MIGRANTS 25
#define MAX_MUTATION_PTS 16
#define MAX_ADDRESS_BITS 32

#define NUM_TRANSLATION_PER_GEN 1
#define NUM_RETAINED_SCORE_PER_GEN 1
#define INJECT_XOR_CANDIDATE 1
#define INJECT_XOR_SENSE_THRESHOLD 0

struct Gene {
    uint32_t m_a;
    uint32_t m_b;
    Gene() {}
    Gene(uint32_t x_a, uint32_t x_b)
        :m_a(x_a), m_b(x_b) {
            rearrange();
        }

    Gene(const Gene& x_gene) {
        m_a = x_gene.m_a;
        m_b = x_gene.m_b;
        rearrange();
    }
    string to_string() {
        string str = std::to_string(m_a) + ", " + std::to_string(m_b);
        return str;
    }

    void rearrange() {
        if (m_a > m_b) {
            uint32_t temp = m_a;
            m_a = m_b;
            m_b = temp;
        } 
    }

    friend bool operator<  (const Gene &g1, const Gene &g2) { return g1.m_a <  g2.m_a; }
    friend bool operator<= (const Gene &g1, const Gene &g2) { return g1.m_a <= g2.m_a; }
    friend bool operator>  (const Gene &g1, const Gene &g2) { return g1.m_a >  g2.m_a; }
    friend bool operator>= (const Gene &g1, const Gene &g2) { return g1.m_a >= g2.m_a; }
    friend bool operator== (const Gene &g1, const Gene &g2) { return ((g1.m_a == g2.m_a) && (g1.m_b == g2.m_b)); }
    friend bool operator!= (const Gene &g1, const Gene &g2) { return ((g1.m_a != g2.m_a) || (g1.m_b != g2.m_b)); }
};

//========================================================================================================================//
// GENETIC ALGORITHM CANDIDATE
//========================================================================================================================//
class Candidate {
public:
    Candidate() {
        reset_score();
        m_sequence = vector<bool>(MAX_ADDRESS_BITS, 0);
    }

    Candidate(vector<uint32_t> x_sequence, bool x_should_sort=true) {
        reset_score();
        m_sequence = vector<bool>(MAX_ADDRESS_BITS, 0);
        initialize(x_sequence, x_should_sort);
    }

    ~Candidate() {}

    Candidate(const Candidate& x_candidate) {
        m_genes          = x_candidate.m_genes;
        m_sequence       = x_candidate.m_sequence;
        m_score          = x_candidate.m_score;
        m_score_hist     = x_candidate.m_score_hist;
        m_dropped_avg    = x_candidate.m_dropped_avg;
        m_num_remain_top = x_candidate.m_num_remain_top;
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

    void compute_fitness_score() {
        // average score with average of past dropped scores
        double sum_score = 0;
        for (uint32_t i=0; i < m_score_hist.size(); ++i)
            sum_score += m_score_hist[i];

        for (uint32_t i=0; i < m_dropped_avg.size(); ++i)
            sum_score += m_dropped_avg[i]*(NUM_TRANSLATION_PER_GEN-NUM_RETAINED_SCORE_PER_GEN);

        m_score = sum_score / (m_score_hist.size() + m_dropped_avg.size()*(NUM_TRANSLATION_PER_GEN-NUM_RETAINED_SCORE_PER_GEN));
    }

    void reset_score(uint32_t x_num_retained_scores=0) {
        m_score_hist.resize(x_num_retained_scores);
        if (x_num_retained_scores == 0)
            m_dropped_avg.resize(0);

        compute_fitness_score();
        m_num_remain_top = 0;
    }

    void drop_scores(bool x_keep_latest) {
        if (m_score_hist.size() == 0)
            return;

        if (NUM_TRANSLATION_PER_GEN == NUM_RETAINED_SCORE_PER_GEN)  // retain all scores, no drop
            return;

        cout << "Candidate: " << this->to_string() << ". Scores: " << vec_to_string(m_score_hist) << endl;
        if (m_score_hist.size() <= NUM_RETAINED_SCORE_PER_GEN) {
            cout << "History size not greater than num retain. history size: " << m_score_hist.size() << ". num retain: " << (NUM_TRANSLATION_PER_GEN-NUM_RETAINED_SCORE_PER_GEN) << endl;
        }
        else {
            // always keep the first score, which is the original mapping
            if (x_keep_latest) {
                // record the average of the dropped scores
                double dropped_avg=0;
                for (auto it=m_score_hist.begin()+1; it != (m_score_hist.begin()+1+NUM_TRANSLATION_PER_GEN-NUM_RETAINED_SCORE_PER_GEN); ++it) {
                    dropped_avg += *it;
                }
                
                m_dropped_avg.push_back(dropped_avg/(NUM_TRANSLATION_PER_GEN-NUM_RETAINED_SCORE_PER_GEN));

                m_score_hist.erase(m_score_hist.begin()+1, m_score_hist.begin()+1+NUM_TRANSLATION_PER_GEN-NUM_RETAINED_SCORE_PER_GEN);
                cout << "Candidate: " << this->to_string() << ". Oldest scores dropped. Scores: " << vec_to_string(m_score_hist) << endl;
            }
            else {
                // record the average of the dropped scores
                double dropped_avg=0;
                for (auto it=m_score_hist.rbegin(); it != (m_score_hist.rbegin()+NUM_TRANSLATION_PER_GEN-NUM_RETAINED_SCORE_PER_GEN); ++it) {
                    dropped_avg += *it;
                }
                
                m_dropped_avg.push_back(dropped_avg/(NUM_TRANSLATION_PER_GEN-NUM_RETAINED_SCORE_PER_GEN));

                m_score_hist.resize(NUM_RETAINED_SCORE_PER_GEN);
                cout << "Candidate: " << this->to_string() << ". Newest scores dropped. Scores: " << vec_to_string(m_score_hist) << endl;
            }
            compute_fitness_score();
        }
    }
    
    void fitness(double x_score) {
        m_score_hist.push_back((x_score));
        compute_fitness_score();
    }

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
        m_genes[x_pos].rearrange();
        sort(m_genes.begin(), m_genes.end());
        return old_val;
    }

    uint32_t modify_gene(uint32_t x_pos, uint32_t x_val) {
        uint32_t old_val = m_genes[x_pos].m_b;
        m_genes[x_pos].m_b = x_val;
        return old_val;
    }

    Gene replace_gene(uint32_t x_pos, Gene x_gene) {
        Gene replaced_gene = m_genes[x_pos];
        m_genes[x_pos] = x_gene;
        sort(m_genes.begin(), m_genes.end());
        return replaced_gene;
    }

    // swap segment m_b with the right next gene
    void swap_gene(uint32_t x_pos) {
        uint32_t temp = m_genes[x_pos].m_b;
        m_genes[x_pos].m_b = m_genes[x_pos+1].m_b;
        m_genes[x_pos+1].m_b = temp;
    }

    void initialize(vector<uint32_t>& x_sequence, bool x_should_sort=true) {
        for (uint32_t i=0; i < x_sequence.size(); i+=2) {
            Gene new_gene = Gene(x_sequence[i+1], x_sequence[i]);
            m_genes.push_back(new_gene);
        }
        for (uint32_t i=0; i < x_sequence.size(); ++i)
            m_sequence[x_sequence[i]] = true;
        if (x_should_sort)
            sort(m_genes.begin(), m_genes.end());
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

    void genes(vector<Gene> x_genes) { m_genes = x_genes; }
    double fitness() { return m_score; }
    vector<Gene> genes() { return m_genes; }
    Gene gene(uint32_t x_pos) { return m_genes[x_pos]; }

    void update_sequence(uint32_t x_pos, bool x_val) { m_sequence[x_pos] = x_val; }
    bool is_in_sequence(uint32_t x_bit) { return m_sequence[x_bit]; }

    uint32_t history_length() { return m_score_hist.size(); }
    uint32_t num_remain_top() { return m_num_remain_top; }

public:
    vector<Gene> m_genes;
    vector<bool> m_sequence;        // sequence of selected bits from all genes 
    double m_score;                 // fitness score
    vector<double> m_score_hist;
    vector<double> m_dropped_avg;   // average of the dropped scores
    uint32_t m_num_remain_top;
};


class CandidateInjector {
public:
    CandidateInjector() {
        // the current injector supports injecting the vanilla XOR-hash candidate
        uint32_t sequence_length = 2*lg2(LLC_SET);
        vector<uint32_t> dna_sequence = rand_gen(sequence_length, (uint32_t)MAX_ADDRESS_BITS, sequence_length/2, true);

        for (uint32_t i=0 ; i < sequence_length; ++i) {
            if (i%2==1) {
                dna_sequence[i] = (sequence_length+i)/2;
            }
        }  

        Candidate injecting_candidate = Candidate(dna_sequence);
        m_injcands.push_back(injecting_candidate);

        cout << "Injecting Candidate: " << m_injcands[0].to_string() << endl;
    }

    bool is_candidate_injected(Candidate& x_candidate) {
        for (uint32_t i=0; i < m_injcands.size(); ++i) {
            if (x_candidate == m_injcands[i])
                return true;
        }
        return false;
    }

public:
    vector<Candidate> m_injcands;
};

//========================================================================================================================//
// GENETIC INDEX
//========================================================================================================================//
class GeneticIndex : public Index {
public:
    GeneticIndex()
    :m_mutation_rate(INIT_MUTATION_RATE), m_emigration_freq(INIT_EMIGRATION_FREQ), current_candidate_idx(0) {
        print_settings();

        uint32_t sequence_length = 2*lg2(LLC_SET);
        cout << "Initializing population. Sequence length: " << sequence_length << endl;
        for (uint32_t i=0; i < NUM_INIT_CANDIDATES; ++i) {
            vector<uint32_t> dna_sequence = rand_gen(sequence_length, (uint32_t)MAX_ADDRESS_BITS);
            Candidate init_candidate = Candidate(dna_sequence);
            m_population.push_back(init_candidate);
        }

        if (INJECT_XOR_CANDIDATE) {
            m_num_injected_cands = m_xorhash_injector.m_injcands.size();
            for (uint32_t i=0; i < m_num_injected_cands; ++i)
                m_population.push_back(m_xorhash_injector.m_injcands[i]);
        }
        
        cout << "Genetic Algorithm initialization completed. Initial population: " << m_population.size() << endl;
    }

    ~GeneticIndex() {}

    uint32_t get_set(uint64_t address);
    void initialize(vector<Access>& history) {}
    bool update(uint64_t address);
    
    // genetic algorithm
    uint32_t population_size() { return m_population.size(); }
    uint32_t num_past_candidates() { return m_past_population.size(); }
    uint32_t curr_candidate_idx() { return current_candidate_idx; }
    void curr_candidate_idx(uint32_t x_idx) {  current_candidate_idx = x_idx; }
    void next_candidate();
    bool evaluate();
    void print_population(bool with_fitness=false);
    void print_candidate(uint32_t x_idx, bool with_fitness=false);
    void print_settings();
    void drop_scores(bool x_first);
    void incr_num_remain_top();

    Candidate get_candidate(uint32_t x_index) { return m_population[x_index]; }


    virtual void crossover(bool mutate_rate_override=false, double x_mutation_rate=0);
    virtual void generate_migrants(uint32_t num_migrants);
    virtual Candidate reproduce(Candidate& x_mom, Candidate& x_dad, bool& is_new_child);
    virtual Candidate mutate_point(Candidate& x_candidate);
    virtual Candidate mutate_swap(Candidate& x_candidate);
    
protected:
    vector<Candidate> m_population;
    Candidate m_best_candidate;
    uint32_t current_candidate_idx;
    double m_mutation_rate;
    double m_emigration_freq;
    CandidateInjector m_xorhash_injector;
    uint32_t m_num_injected_cands;

    vector<Candidate> m_past_population;
};

//========================================================================================================================//
// FIXED INDEX GENETIC INDEX
//========================================================================================================================//
class FixedIndexGenetic : public GeneticIndex {
public:
    FixedIndexGenetic()
    :GeneticIndex() {
        m_population.resize(0);
        uint32_t sequence_length = 2*lg2(LLC_SET);
        cout << "Initializing population. Sequence length: " << sequence_length << endl;
        for (uint32_t i=0; i < NUM_INIT_CANDIDATES; ++i) {
            vector<uint32_t> dna_sequence = rand_gen(sequence_length, (uint32_t)MAX_ADDRESS_BITS, sequence_length/2, true);
            Candidate init_candidate = Candidate(dna_sequence);
            m_population.push_back(init_candidate);
        }
        
        if (INJECT_XOR_CANDIDATE) {
            m_num_injected_cands = m_xorhash_injector.m_injcands.size();
            for (uint32_t i=0; i < m_num_injected_cands; ++i)
                m_population.push_back(m_xorhash_injector.m_injcands[i]);
        }

        cout << "Genetic Algorithm initialization completed. Initial population: " << m_population.size() << endl;
    }

    void generate_migrants(uint32_t num_migrants);
    Candidate reproduce(Candidate& x_mom, Candidate& x_dad, bool& is_new_child);
    Candidate mutate_point(Candidate& x_candidate);

protected:

};

//========================================================================================================================//
// SELECTIVE GENETIC INDEX
//========================================================================================================================//
class SelectiveGenetic : public GeneticIndex {
public:
    SelectiveGenetic()
    :GeneticIndex() {
        m_population.resize(0);

        // hard-coded selected bit pool
        vector<uint32_t> bit0 = {34,35,22,32};
        vector<uint32_t> bit1 = {36,22,38,32};
        vector<uint32_t> bit2 = {14,26,34,31};
        vector<uint32_t> bit3 = {38,28,22,14};
        vector<uint32_t> bit4 = {18,32,14,35};
        vector<uint32_t> bit5 = {15,22,29,36};
        vector<uint32_t> bit6 = {37,26,22,38};
        vector<uint32_t> bit7 = {29,33,18,22};
        vector<uint32_t> bit8 = {14,36,26,35};
        vector<uint32_t> bit9 = {21,29,34,22};
        vector<uint32_t> bit10 = {29,32,34,26};
        vector<uint32_t> bit11 = {31,38,14,33};
        vector<uint32_t> bit12 = {35,36,3,5};

        m_selected_bitpool = {bit0, bit1, bit2, bit3, bit4, bit5, bit6,
                              bit7, bit8, bit9, bit10, bit11, bit12};

        // initialize population
        cout << "Initializing population." << endl;
        for (uint32_t i=0; i < NUM_INIT_CANDIDATES; ++i) {
            vector<uint32_t> dna_sequence = dna_generator(m_selected_bitpool);
            Candidate init_candidate = Candidate(dna_sequence, false);
            m_population.push_back(init_candidate);

            cout << "dna sequence: ";
            for (uint32_t i=0; i < dna_sequence.size(); ++i)
                cout << dna_sequence[i] << " ";
            cout << endl;

            cout << "candidate: " << init_candidate.to_string() << endl;
        }
        cout << "Genetic Algorithm initialization completed. Initial population: " << m_population.size() << endl;
    }

    ~SelectiveGenetic() {}
    void crossover(bool mutate_rate_override=false, double x_mutation_rate=0);
    void generate_migrants(uint32_t num_migrants) {}
    Candidate mutate_point(Candidate& x_candidate);
    vector<uint32_t> dna_generator(vector<vector<uint32_t>>& x_pool);

protected:
    vector<vector<uint32_t>> m_selected_bitpool;
};



//========================================================================================================================//
// INCREMENTAL GENETIC INDEX
//========================================================================================================================//
class IncrementalGenetic : public Index {
public:
    IncrementalGenetic()
        :m_interval_size(5000), m_curr_eval_bit(0), m_evaluation_done(false) {

        // hard-coded selected bit pool
        vector<uint32_t> bit0 = {34,35,22,32};
        vector<uint32_t> bit1 = {36,22,38,32};
        vector<uint32_t> bit2 = {14,26,34,31};
        vector<uint32_t> bit3 = {38,28,22,14};
        vector<uint32_t> bit4 = {18,32,14,35};
        vector<uint32_t> bit5 = {15,22,29,36};
        vector<uint32_t> bit6 = {37,26,22,38};
        vector<uint32_t> bit7 = {29,33,18,22};
        vector<uint32_t> bit8 = {14,36,26,35};
        vector<uint32_t> bit9 = {21,29,34,22};
        vector<uint32_t> bit10 = {29,32,34,26};
        vector<uint32_t> bit11 = {31,38,14,33};
        vector<uint32_t> bit12 = {35,36,3,5};

        m_selected_bitpool = {bit0, bit1, bit2, bit3, bit4, bit5, bit6,
                              bit7, bit8, bit9, bit10, bit11, bit12};
        
        m_curr_config = vector<uint32_t>(m_selected_bitpool.size(), 0);
        m_scoreboard  = vector<uint32_t>(m_selected_bitpool[m_curr_eval_bit].size(), 0);

    }

    ~IncrementalGenetic() {}

    uint32_t get_set(uint64_t address);
    void initialize(vector<Access>& history) {}
    bool update(uint64_t address);
    void move_cache_data(); 
    

protected:
    vector<vector<uint32_t>> m_selected_bitpool;
    vector<uint32_t> m_curr_config;  // 13 indices, one per bitpool
    uint32_t m_interval_size;
    uint32_t m_curr_eval_bit;
    vector<uint32_t> m_scoreboard;
    bool m_evaluation_done;
};


#endif // GENETIC_INDEX_H