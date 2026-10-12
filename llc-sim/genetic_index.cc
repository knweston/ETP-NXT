#include "genetic_index.h"


uint32_t GeneticIndex::get_set(uint64_t address) {
    uint32_t hashed_index = 0;
    vector<Gene> current_genes = m_population[current_candidate_idx].genes();

    for (uint32_t i=0; i < current_genes.size(); ++i) {
        uint32_t selected_bit_a = (address >> current_genes[i].m_a) & 1;
        uint32_t selected_bit_b = (address >> current_genes[i].m_b) & 1;
        hashed_index = (hashed_index << 1) | (selected_bit_a ^ selected_bit_b);
    }
    return hashed_index;
}

bool GeneticIndex::update(uint64_t address) {
    return false;
}

// genetic algorithm
bool GeneticIndex::evaluate() {
    // sort based on fitness
    sort(m_population.begin(), m_population.end());
    
    // remove underperformed candidates, keep the top performers
    uint32_t drop_idx_from_last = 1;
    this->incr_num_remain_top();
    cout << "\nLeaderboard: " << endl;
    print_population(true);
    cout << endl;

    while(m_population.size() > TOP_CANDIDATE_CUTOFF) {
        if (INJECT_XOR_CANDIDATE && (m_num_injected_cands > 0)) {
            if (!m_xorhash_injector.is_candidate_injected(m_population[m_population.size()-drop_idx_from_last])) {
                m_past_population.push_back(m_population[m_population.size()-drop_idx_from_last]);   // record past candidates
                m_population.erase(m_population.end()-drop_idx_from_last);
            }
            else
                ++drop_idx_from_last;
        }
        else {
            m_past_population.push_back(m_population[m_population.size()-1]);   // record past candidates
            m_population.pop_back();
        }
    }

    // save the current best
    if (!m_xorhash_injector.is_candidate_injected(m_population[0]))
        m_best_candidate = m_population[0];
    else
        m_best_candidate = m_population[1];

    // remove candidates that perform worse than XOR-hash (injected candidate)
    uint32_t injected_idx = 0;
    for (uint32_t i=0; i < m_population.size(); ++i) {
        if (m_xorhash_injector.is_candidate_injected(m_population[i])) {
            injected_idx = i;
            break;
        }
    }

    // remove candidates that are not better than XOR by a certain amount (INJECT_XOR_SENSE_THRESHOLD)
    double score_threshold = m_population[injected_idx].m_score*(100-INJECT_XOR_SENSE_THRESHOLD)/100;
    uint32_t idx_from_first = 0;
    while (idx_from_first < m_population.size()) {
        if (!m_xorhash_injector.is_candidate_injected(m_population[idx_from_first])) {
            if (m_population[idx_from_first].m_score > score_threshold) {
                cout << "Remove due to worse performance than Injected XOR (threshold: " << score_threshold << "): " << m_population[idx_from_first].to_string() << endl;
                m_past_population.push_back(m_population[idx_from_first]);   // record past candidates
                m_population.erase(m_population.begin()+idx_from_first);
            }
            else
                idx_from_first++;
        }
        else
            idx_from_first++;
    }
    
    // after all the filtering, if the population <= 1, add back the previous best to reproduce
    if (m_population.size() <= 1 && !m_xorhash_injector.is_candidate_injected(m_best_candidate))
        m_population.push_back(m_best_candidate);

    // reset the curr_candidate_idx
    current_candidate_idx = m_population.size();

    // remove some top candidates to avoid local maxima
    if ((m_emigration_freq != 0) && ((rand() % (uint32_t)(1/m_emigration_freq)) == 0)) {
        while(m_population.size() > POPULATION_AFTER_EMIGRATION) {
            m_past_population.push_back(m_population[m_population.size()-1]);   // record past candidates
            m_population.pop_back();
        }
        
        // reset the curr_candidate_idx
        current_candidate_idx = m_population.size();

        return true;
    }
    return false;
}

void GeneticIndex::crossover(bool mutate_rate_override, double x_mutation_rate) {
    double mutation_rate = (mutate_rate_override? x_mutation_rate : m_mutation_rate);
    vector<Candidate> children;
    cout << "[Genetic Algorithm] reproducing offsprings..." << endl;
    // reproduce offsprings
    for (uint32_t m=0; m < m_population.size(); ++m) {
        // if the injected candidate is the worst performer in the toplist, skip it
        if (INJECT_XOR_CANDIDATE && (m_num_injected_cands > 0) 
                                 && m_xorhash_injector.is_candidate_injected(m_population[m])
                                 && (m == (m_population.size()-1))
                                 && (m_population.size() > 1)) 
        {
            cout << "Skip reproduce for injected candidate due to poor performance: " << m_population[m].to_string() << endl;
            continue;
        }

        for (uint32_t d=m+1; d < m_population.size(); ++d) {
            // if the injected candidate is the worst performer in the toplist, skip it
            if (INJECT_XOR_CANDIDATE && (m_num_injected_cands > 0) 
                                     && m_xorhash_injector.is_candidate_injected(m_population[d])
                                     && (d == (m_population.size()-1))
                                     && (m_population.size() > 1)) 
            {
                cout << "Skip reproduce for injected candidate due to poor performance: " << m_population[d].to_string() << endl;
                continue;
            }
            bool is_new_child = false;
            Candidate new_child = reproduce(m_population[m], m_population[d], is_new_child);
            if (is_new_child)
                children.push_back(new_child);
        }
    }

    cout << "[Genetic Algorithm] generating mutants..." << endl;
    // generate mutations
    for (uint32_t i=0; i < m_population.size(); ++i) {
        cout << "candidate " << i+1 << "... ";
        if ((rand()%100) < (uint32_t)(mutation_rate*100)) {
            cout << "mutated (point)!\n";
            children.push_back(mutate_point(m_population[i]));
            cout << "   original: " << m_population[i].to_string() << endl;
            cout << "   mutant:   " << children[children.size()-1].to_string() << endl;
        }
        else {
            cout << "normal.\n";
        }
    }

    cout << "\n[Genetic Algorithm] total children generated: " << children.size() << endl;
    for (uint32_t i=0; i < children.size(); ++i) {
        cout << children[i].to_string() << endl;
    }

    for (uint32_t i=0; i < children.size(); ++i) {
        bool is_duplicate = false;
        for (uint32_t j=0; j < m_population.size(); ++j) {
            if (children[i] == m_population[j]) {
                is_duplicate = true;
                break;
            } 
        }
        if (!is_duplicate) {
            for (uint32_t j=0; j < m_past_population.size(); ++j) {
                if (children[i] == m_past_population[j]) {
                    is_duplicate = true;
                    break;
                }
            }
        }
        if (!is_duplicate)
            m_population.push_back(children[i]);
    }
}

Candidate GeneticIndex::reproduce(Candidate& x_mom, Candidate& x_dad, bool& is_new_child) {
    Candidate child = x_mom;
    child.reset_score();
    vector<Gene> dad_genes = x_dad.genes();
    for (uint32_t i=0; i < dad_genes.size(); ++i) {
        if (!(child.is_in_sequence(dad_genes[i].m_a)) && !(child.is_in_sequence(dad_genes[i].m_b))) {
            Gene replaced_gene = child.replace_gene(i, dad_genes[i]);
            child.update_sequence(replaced_gene.m_a, false);
            child.update_sequence(replaced_gene.m_b, false);
            child.update_sequence(dad_genes[i].m_a, true);
            child.update_sequence(dad_genes[i].m_b, true);
            is_new_child = true;
        }
    }
    return child;
}

Candidate GeneticIndex::mutate_point(Candidate& x_candidate) {
    Candidate new_candidate = x_candidate;
    new_candidate.reset_score();
    uint32_t num_mutation_pts = rand() % MAX_MUTATION_PTS + 1;
    vector<uint32_t> mutating_pts = rand_gen(num_mutation_pts, (uint32_t)(2*lg2(LLC_SET)));

    cout << "num mutation pts: " << num_mutation_pts << ": ";
    for (uint32_t pt=0; pt < mutating_pts.size(); ++pt) {
        cout << mutating_pts[pt] << " ";
    }
    cout << endl;

    for (uint32_t pt=0; pt < mutating_pts.size(); ++pt) {
        uint32_t mutated_val = MAX_ADDRESS_BITS;
        do {
            mutated_val = rand() % MAX_ADDRESS_BITS;
        }
        while (new_candidate.is_in_sequence(mutated_val));
        uint32_t old_val = new_candidate.modify_gene(floor(mutating_pts[pt]/2), mutated_val, mutating_pts[pt] % 2);
        new_candidate.update_sequence(old_val, false);
        new_candidate.update_sequence(mutated_val, true);
    }
    return new_candidate;
}

Candidate GeneticIndex::mutate_swap(Candidate& x_candidate) {
    Candidate new_candidate = x_candidate;
    new_candidate.reset_score();
    uint32_t num_swaps = rand() % (MAX_MUTATION_PTS/2) + 1;
    vector<uint32_t> swapping_pts = rand_gen(num_swaps, (uint32_t)(lg2(LLC_SET)-1));
    cout << "num swappings: " << num_swaps << ": ";
    for (uint32_t pt=0; pt < swapping_pts.size(); ++pt) {
        cout << swapping_pts[pt] << " ";
    }
    cout << endl;

    // swap m_b with the neighbor gene
    for (uint32_t pt=0; pt < swapping_pts.size(); ++pt) {
        new_candidate.swap_gene(swapping_pts[pt]);
    }
    return new_candidate;
}

void GeneticIndex::generate_migrants(uint32_t num_migrants) {
    uint32_t sequence_length = 2*lg2(LLC_SET);
    cout << "\n[Genetic Algorithm] num random migrants generated: " << num_migrants << endl;
    for (uint32_t i=0; i < num_migrants; ++i) {
        vector<uint32_t> dna_sequence = rand_gen(sequence_length, (uint32_t)MAX_ADDRESS_BITS);
        Candidate new_migrant = Candidate(dna_sequence);
        m_population.push_back(new_migrant);
    }
}

void GeneticIndex::next_candidate() {
    // m_population[current_candidate_idx].fitness(MISSES[LOAD] + MISSES[RFO] + MISSES[PREFETCH]);
    m_population[current_candidate_idx].fitness(MISSES[LOAD] + MISSES[RFO]);
    ++current_candidate_idx;
}

void GeneticIndex::print_population(bool with_fitness) {
    for (uint32_t i=0; i < m_population.size(); ++i)
        print_candidate(i, with_fitness);
}

void GeneticIndex::print_candidate(uint32_t x_idx, bool with_fitness) {
    if (with_fitness)
        cout << std::fixed << m_population[x_idx].to_string() << " [score: " << (uint32_t)m_population[x_idx].fitness() << "]" 
                                                              << " [hist_len: " << (uint32_t)m_population[x_idx].m_score_hist.size() << "]" 
                                                              << " [num_in_top: " << m_population[x_idx].num_remain_top() << "]" 
                                                              << endl;
    else
        cout << m_population[x_idx].to_string() << endl;
}

void GeneticIndex::drop_scores(bool x_keep_latest) {
    for (uint32_t i=0; i < m_population.size(); ++i) {
        m_population[i].drop_scores(x_keep_latest);
    }
}

void GeneticIndex::print_settings() {
    cout << "========================================================================" << endl;
    cout << "INDEXING_SCHEME: " << INDEXING_SCHEME << endl;
    cout << "NUM_GENERATIONS: " << NUM_GENERATIONS << endl;
    cout << "TOP_CANDIDATE_CUTOFF: " << TOP_CANDIDATE_CUTOFF << endl;
    cout << "POPULATION_AFTER_EMIGRATION: " << POPULATION_AFTER_EMIGRATION << endl;
    cout << "NUM_INIT_CANDIDATES: " << NUM_INIT_CANDIDATES << endl;
    cout << "INIT_EMIGRATION_FREQ: " << INIT_EMIGRATION_FREQ << endl;
    cout << "INIT_MUTATION_RATE: " << INIT_MUTATION_RATE << endl;
    cout << "MAX_MIGRANTS: " << MAX_MIGRANTS << endl;
    cout << "MAX_MUTATION_PTS: " << MAX_MUTATION_PTS << endl;
    cout << "MAX_ADDRESS_BITS: " << MAX_ADDRESS_BITS << endl;
    cout << "NUM_TRANSLATION_PER_GEN: " << NUM_TRANSLATION_PER_GEN << endl;
    cout << "NUM_RETAINED_SCORE_PER_GEN: " << NUM_RETAINED_SCORE_PER_GEN << endl;
    cout << "INJECT_XOR_CANDIDATE: " << (INJECT_XOR_CANDIDATE? "Yes" : "No") << endl;
    cout << "INJECT_XOR_SENSE_THRESHOLD: " << INJECT_XOR_SENSE_THRESHOLD << endl; 
    cout << "========================================================================" << endl;
}

void GeneticIndex::incr_num_remain_top() {
    uint32_t num_incr = (m_population.size() > TOP_CANDIDATE_CUTOFF? TOP_CANDIDATE_CUTOFF : m_population.size());
    for (uint32_t i=0; i < num_incr; ++i)
        m_population[i].m_num_remain_top++;
}

//========================================================================================================================//
// FIXED INDEX GENETIC INDEX
//========================================================================================================================//
void FixedIndexGenetic::generate_migrants(uint32_t num_migrants) {
    uint32_t sequence_length = 2*lg2(LLC_SET);
    cout << "\n[Genetic Algorithm] num random migrants generated: " << num_migrants << endl;
    for (uint32_t i=0; i < num_migrants; ++i) {
        vector<uint32_t> dna_sequence = rand_gen(sequence_length, (uint32_t)MAX_ADDRESS_BITS, sequence_length/2, true);
        Candidate new_migrant = Candidate(dna_sequence);
        m_population.push_back(new_migrant);
    }
}

Candidate FixedIndexGenetic::reproduce(Candidate& x_mom, Candidate& x_dad, bool& is_new_child) {
    Candidate child = x_mom;
    child.reset_score();
    vector<Gene> dad_genes = x_dad.genes();
    for (uint32_t i=0; i < dad_genes.size(); ++i) {
        if (!child.is_in_sequence(dad_genes[i].m_b)) {
            Gene replaced_gene = child.replace_gene(i, dad_genes[i]);
            child.update_sequence(replaced_gene.m_b, false);
            child.update_sequence(dad_genes[i].m_b, true);
            is_new_child = true;
        }
    }
    return child;
}

Candidate FixedIndexGenetic::mutate_point(Candidate& x_candidate) {
    Candidate new_candidate = x_candidate;
    new_candidate.reset_score();
    uint32_t num_mutation_pts = rand() % (MAX_MUTATION_PTS/2) + 1;
    vector<uint32_t> mutating_pts = rand_gen(num_mutation_pts, (uint32_t)(lg2(LLC_SET)));
    cout << "num mutation pts: " << num_mutation_pts << ": ";
    for (uint32_t pt=0; pt < mutating_pts.size(); ++pt) {
        cout << mutating_pts[pt] << " ";
    }
    cout << endl;

    uint32_t lg2_num_set = lg2(LLC_SET);
    for (uint32_t pt=0; pt < mutating_pts.size(); ++pt) {
        uint32_t mutated_val = MAX_ADDRESS_BITS;
        do {
            mutated_val = rand() % (MAX_ADDRESS_BITS-lg2_num_set) + lg2_num_set;
        }
        while (new_candidate.is_in_sequence(mutated_val));
        uint32_t old_val = new_candidate.modify_gene(mutating_pts[pt], mutated_val);
        new_candidate.update_sequence(old_val, false);
        new_candidate.update_sequence(mutated_val, true);
    }
    return new_candidate;
}


//========================================================================================================================//
// SELECTIVE GENETIC INDEX
//========================================================================================================================//
void SelectiveGenetic::crossover(bool mutate_rate_override, double x_mutation_rate) {
    vector<Candidate> children;

    // generate point mutations
    cout << "[Genetic Algorithm] generating mutants..." << endl;
    for (uint32_t i=0; i < m_population.size(); ++i) {
        cout << "candidate " << i+1 << "... ";
        // if ((rand() % (uint32_t)(1/m_mutation_rate)) == 0) {
        if ((rand()%100) < (uint32_t)(m_mutation_rate*100)) {
            cout << "mutated (point)!\n";
            children.push_back(mutate_point(m_population[i]));
            cout << "   original: " << m_population[i].to_string() << endl;
            cout << "   mutant:   " << children[children.size()-1].to_string() << endl;
        }
        else {
            cout << "normal.\n";
        }
    }

    cout << "\n[Genetic Algorithm] total children generated: " << children.size() << endl;
    for (uint32_t i=0; i < children.size(); ++i) {
        cout << children[i].to_string() << endl;
    }

    // deduplicate children and add to population
    for (uint32_t i=0; i < children.size(); ++i) {
        bool is_duplicate = false;
        for (uint32_t j=0; j < m_population.size(); ++j) {
            if (children[i] == m_population[j]) {
                is_duplicate = true;
                break;
            } 
        }
        if (!is_duplicate)
            m_population.push_back(children[i]);
    }
}

Candidate SelectiveGenetic::mutate_point(Candidate& x_candidate) {
    Candidate new_candidate = x_candidate;
    new_candidate.reset_score();
    uint32_t num_mutations = rand() % (MAX_MUTATION_PTS/2) + 1;
    vector<uint32_t> mutating_pts = rand_gen(num_mutations, (uint32_t)(lg2(LLC_SET)));
    cout << "num_mutations: " << num_mutations << ": ";
    for (uint32_t pt=0; pt < mutating_pts.size(); ++pt) {
        cout << mutating_pts[pt] << " ";
    }
    cout << endl;

    for (uint32_t pt=0; pt < mutating_pts.size(); ++pt) {        
        uint32_t new_idx = 0;
        do {
            new_idx = rand() % m_selected_bitpool[mutating_pts[pt]].size();
        }
        while (m_selected_bitpool[mutating_pts[pt]][new_idx] == new_candidate.gene(mutating_pts[pt]).m_b);
        uint32_t old_val = new_candidate.modify_gene(mutating_pts[pt], m_selected_bitpool[mutating_pts[pt]][new_idx]);
    }
    return new_candidate;
}

vector<uint32_t> SelectiveGenetic::dna_generator(vector<vector<uint32_t>>& x_pool) {
    vector<uint32_t> dna_sequence;
    for (uint32_t i=0; i < x_pool.size(); ++i) {
        uint32_t random_idx = rand() % x_pool[i].size();
        dna_sequence.push_back(i);
        dna_sequence.push_back(x_pool[i][random_idx]);
    }
    cout << endl;
    return dna_sequence;
}

//========================================================================================================================//
// INCREMENTAL GENETIC INDEX
//========================================================================================================================//
uint32_t IncrementalGenetic::get_set(uint64_t address) {
    uint32_t hashed_index = 0;
    for (uint32_t i=0; i < m_selected_bitpool.size(); ++i) {
        uint32_t selected_bit_a = (address >> i) & 1;
        uint32_t selected_bit_b = (address >> m_selected_bitpool[i][m_curr_config[i]]) & 1;
        hashed_index = (hashed_index << 1) | (selected_bit_a ^ selected_bit_b);
    }
    return hashed_index;
}


bool IncrementalGenetic::update(uint64_t address) {
    if ((WARM_INST > 0) && (CURRENT_PTR < WARM_INST)) {
        return false;
    }

    if ((CURRENT_PTR % m_interval_size == 0) && (CURRENT_PTR > WARM_INST)) {
        if (!m_evaluation_done) {
            // record miss count of current interval
            m_scoreboard[m_curr_config[m_curr_eval_bit]] = sum(INTERVAL_MISSCOUNT, LLC_SET);
            
            cout << "bit #: " << m_curr_eval_bit << ". ";
            cout << "candidate: " << m_selected_bitpool[m_curr_eval_bit][m_curr_config[m_curr_eval_bit]] << ". ";
            cout << "miss count: " << m_scoreboard[m_curr_config[m_curr_eval_bit]] << endl;
            cout << "indices: ";
            for (uint32_t i=0; i < m_curr_config.size(); ++i) {
                cout << m_curr_config[i] << ", ";
            }
            cout << endl;

            // if done with all candidate of a bit, pick the best one and move to the next bit
            if (m_curr_config[m_curr_eval_bit] == m_selected_bitpool[m_curr_eval_bit].size()-1) {
                cout << endl;
                uint32_t optimal_candidate = 0;
                uint32_t min_misscount = m_scoreboard[0];
                for (uint32_t cid=1; cid < m_scoreboard.size(); ++cid) {
                    if (m_scoreboard[cid] < min_misscount) {
                        min_misscount = m_scoreboard[cid];
                        optimal_candidate = cid;
                    }
                }
                // pick the best candidate for this bit, then move to the next bit
                cout << "optimal candidate: " << optimal_candidate << endl;
                m_curr_config[m_curr_eval_bit] = optimal_candidate;
                m_curr_eval_bit++;

                // if this is the last bit, set eval done flag. Else, reset the scoreboard
                if (m_curr_eval_bit == m_selected_bitpool.size()) {
                    m_evaluation_done = true;
                    cout << "Training done. Best candidate: ";
                    for (uint32_t i=0; i < m_selected_bitpool.size(); ++i) {
                        cout << i << ", " << m_selected_bitpool[i][m_curr_config[i]] << ", ";
                    }
                    cout << endl;
                    cout << "indices: ";
                    for (uint32_t i=0; i < m_curr_config.size(); ++i) {
                        cout << m_curr_config[i] << ", ";
                    }
                    cout << endl;
                }
                else
                    m_scoreboard = vector<uint32_t>(m_selected_bitpool[m_curr_eval_bit].size(), 0);
            }

            // else, move to the next candidate of the current evaluating bit
            else {
                m_curr_config[m_curr_eval_bit]++;
            }
            // reset interval statistics
            memset(INTERVAL_MISSCOUNT, 0, sizeof(INTERVAL_MISSCOUNT));
            move_cache_data();
            return true;
        }
    }

    return false;
}


void IncrementalGenetic::move_cache_data() {
        uint64_t mirrored_cache[LLC_SET][LLC_WAY];
        for (uint32_t set=0; set < LLC_SET; ++set) {
            for (uint32_t way=0; way < LLC_WAY; ++way)
                mirrored_cache[set][way] = 0;
        }

        for (uint32_t set=0; set < LLC_SET; ++set) {
            for (uint32_t way=0; way < LLC_WAY; ++way) {
                uint32_t set_index = get_set(CACHE[set][way]);
                bool is_drop = true;
                for (uint32_t i=0; i < LLC_WAY; ++i) {
                    if (mirrored_cache[set_index][i] == 0) {
                        mirrored_cache[set_index][i] = CACHE[set][way];
                        is_drop = false;
                        break;
                    }
                }
                if (is_drop)
                    NUM_DROPS++;
            }
        }
        
        for (uint32_t set=0; set < LLC_SET; ++set) {
            for (uint32_t way=0; way < LLC_WAY; ++way)
                CACHE[set][way] = mirrored_cache[set][way];
        }
}