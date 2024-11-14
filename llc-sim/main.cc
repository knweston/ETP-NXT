#include "belady.h"
#include "lru.h"
#include "drrip.h"
#include "frequency_index.h"
#include "normal_index.h"
#include "neural_index.h"
#include "genetic_index.h"
#include "xorhash_index.h"

//========================================================================================================================//
// GLOBAL VARIABLES
//========================================================================================================================//
vector<Access> GLOBAL_HISTORY;
vector<Translation> GLOBAL_PAGETABLE;

uint64_t CACHE[LLC_SET][LLC_WAY];
uint64_t ACCESSES[NUM_TYPES];
uint64_t HITS[NUM_TYPES];
uint64_t MISSES[NUM_TYPES];
uint64_t EVICTIONS[LLC_SET];
uint64_t INTERVAL_EVICTIONS[LLC_SET];
uint64_t INTERVAL_MISSCOUNT[LLC_SET];

uint32_t NUM_THREADS     = LLC_WAY > MAX_THREADS ? MAX_THREADS : LLC_WAY;
uint32_t PRINT_INTERVAL  = 0;
uint64_t CURRENT_PTR     = 0;
uint32_t CHECKPOINT      = 0;
uint64_t NUM_DROPS       = 0;
uint64_t NUM_LOOPS       = 0;
uint64_t MAX_INST        = 0;
uint64_t WARM_INST       = 0;
string   TRACE_FILE      = "";
string   INDEXING_SCHEME = "NORMAL";
string   REPL_POLICY     = "LRU";
string   CONFIG_FILE     = "";

uint32_t RAND_SEED       = 0;
RANDOM   RAND_ENGINE(RAND_SEED);    // seed of champsim random class is zero

void main_loop(Replacement *replacement_policy, Index *indexing) {
    // start the main loop
    auto start = chrono::steady_clock::now();

    // main LLC simulation
    for (; CURRENT_PTR < GLOBAL_HISTORY.size(); ++CURRENT_PTR) {
        if ((MAX_INST > 0) && (CURRENT_PTR >= MAX_INST))
            break;

        if ((WARM_INST > 0) && (CURRENT_PTR == WARM_INST)) {
            reset_statistics(true);
            cout << "warmup completed" << endl;
        }
            
        // Save a checkpoint (if CHECKPOINT is ON)
        if (CHECKPOINT && (CURRENT_PTR % CHECKPOINT == 0)) {   
            string filename;
            for (uint32_t i=0; i < TRACE_FILE.size()-13; ++i)
                filename += TRACE_FILE[i];
            filename += "CHECKPOINT.txt";
            ofstream cp_file(filename);
            create_checkpoint(cp_file);               // save simulation data and settings 
            replacement_policy->save_state(cp_file);  // save replacement policy data
            cout << "  Checkpoint saved" << endl;
            cp_file.close();
        } 

        // get current address
        // uint64_t current_addr = GLOBAL_HISTORY[CURRENT_PTR].address;
        uint64_t current_addr = translate_addr(GLOBAL_HISTORY[CURRENT_PTR].address);

        // print simulation progress
        if (PRINT_INTERVAL && (CURRENT_PTR % PRINT_INTERVAL == 0)) {
            auto end = chrono::steady_clock::now();
            cout << left << "  Processed Accesses = " << setw(12) << CURRENT_PTR;
            cout << "  Interval time (s) = " 
                 << chrono::duration_cast<chrono::seconds>(end - start).count()
                 << " s" << endl;
            start = end;
        }

        // get accessing set
        uint32_t set_index = indexing->get_set(current_addr);

        // check whether this is a hit or miss
        int hit_way = check_hit(current_addr, set_index);

        if (hit_way >= 0) { // LLC hit           
            // collect stats
            HITS[GLOBAL_HISTORY[CURRENT_PTR].type]++;
            ACCESSES[GLOBAL_HISTORY[CURRENT_PTR].type]++;

            // update replacement policy
            if ((REPL_POLICY == "LRU" || REPL_POLICY == "DRRIP") && (GLOBAL_HISTORY[CURRENT_PTR].type == WRITEBACK)) {} // writeback hit does not update LRU state
            else
                replacement_policy->update(current_addr, set_index, hit_way, true);
        }

        else { // LLC miss
            if (set_index < LLC_SET) {
                uint32_t victim_way = replacement_policy->get_victim(current_addr, set_index, CACHE[set_index]);

                // collect stats
                MISSES[GLOBAL_HISTORY[CURRENT_PTR].type]++;
                INTERVAL_MISSCOUNT[set_index]++;
                ACCESSES[GLOBAL_HISTORY[CURRENT_PTR].type]++;
                if (CACHE[set_index][victim_way]) {
                    EVICTIONS[set_index]++;
                    INTERVAL_EVICTIONS[set_index]++;
                }
                // fill cache with new data
                CACHE[set_index][victim_way] = current_addr;

                // update replacement policy
                replacement_policy->update(current_addr, set_index, victim_way, false);
            }
            else {
                // collect stats
                MISSES[GLOBAL_HISTORY[CURRENT_PTR].type]++;
                INTERVAL_MISSCOUNT[set_index]++;
                ACCESSES[GLOBAL_HISTORY[CURRENT_PTR].type]++;
                cout << "Invalid set index: " << set_index << endl;
                exit(1);
            }
        }

        bool new_interval = indexing->update(current_addr);
        if (new_interval)
            replacement_policy->reset();

    } // end main simulation

    // print LLC final statistics
    print_final_stats();
}

Replacement* init_replacement_policy(Index* indexing) {
    Replacement* replacement_policy;
    
    if (REPL_POLICY == "BELADY")
        replacement_policy = new Belady();
    else if (REPL_POLICY == "LRU")
        replacement_policy = new LRU();
    else if (REPL_POLICY == "DRRIP")
        replacement_policy = new DRRIP();
    else {
        cout << "Unrecognized replacement policy: " << REPL_POLICY << endl;
        exit(1);
    }
    replacement_policy->initialize(GLOBAL_HISTORY, *indexing);
    cout << "Replacement policy initialization complete" << endl;
    return replacement_policy;
}

Index* init_indexing_scheme() {
    Index* indexing;
    if (INDEXING_SCHEME == "FBI") 
        indexing = new FrequencyIndex();
    else if (INDEXING_SCHEME == "NORMAL")
        indexing = new NormalIndex();
    else if (INDEXING_SCHEME == "NEURAL")
        indexing = new NeuralIndex();
    else if (INDEXING_SCHEME == "GENETIC")
        indexing = new GeneticIndex();
    else if (INDEXING_SCHEME == "SELECTIVE_GENETIC")
        indexing = new SelectiveGenetic();
    else if (INDEXING_SCHEME == "FIXED_IDX_GENETIC")
        indexing = new FixedIndexGenetic();
    else if (INDEXING_SCHEME == "INCREMENTAL")
        indexing = new IncrementalGenetic();
    else if (INDEXING_SCHEME == "CUSTOM_XOR")
        indexing = new CustomXorIndex();
    else if (INDEXING_SCHEME == "XOR")
        indexing = new XorHashIndex();
    else if (INDEXING_SCHEME == "PRIME")
        indexing = new PrimeHashIndex();
    else if (INDEXING_SCHEME == "PROFILE") {
        indexing->profile(GLOBAL_HISTORY);
        cout << "Index Profiling complete" << endl;
        return 0;   // Profile addresses then finish simulation
    }
    else {
        cout << "Unrecognized indexing scheme: " << INDEXING_SCHEME << endl;
        exit(1);
    }
    indexing->initialize(GLOBAL_HISTORY);
    cout << "Index initialization complete" << endl;
    return indexing;
}

//========================================================================================================================//
// MAIN FUNCTION
//========================================================================================================================//
int main(int argc, char *argv[]) {
    if (argc < 9) {
        cout << "Usage: " << argv[0] << " [REPL_POLICY] [INDEXING_SCHEME] [TRACE_FILE] [CHECKPOINT] [PRINT_INTERVAL] [NUM_LOOPS] [MAX_INST] [WARM_INST] [CONFIG_FILE]" << endl;
        exit(1);
    }

    // set simulation parameters
    REPL_POLICY     = argv[1];
    INDEXING_SCHEME = argv[2];
    TRACE_FILE      = argv[3];
    CHECKPOINT      = stoi(argv[4]);
    PRINT_INTERVAL  = stoi(argv[5]);
    NUM_LOOPS       = stoi(argv[6]);
    MAX_INST        = stoi(argv[7]);
    WARM_INST       = stoi(argv[8]);

    if (argc > 9) {
        CONFIG_FILE = argv[9];
    }

    // sanity check
    if (INDEXING_SCHEME == "NEURAL" && argc < 10) {
        cout << "Index " << INDEXING_SCHEME << "requires a socket configuration file" << endl;
        cout << "Usage: " << argv[0] << " [REPL_POLICY] [INDEXING_SCHEME] [TRACE_FILE] [CHECKPOINT] [PRINT_INTERVAL] [NUM_LOOPS] [MAX_INST] [WARM_INST] [CONFIG_FILE]" << endl;
        exit(1);
    }

    // initialize random seed
    uint32_t appname_startpt = 0;
    for (uint32_t i=0; i<TRACE_FILE.size(); ++i) {
        if (TRACE_FILE[i] == '/')
            appname_startpt = i+1;
    }

    for (uint32_t i=appname_startpt; i<TRACE_FILE.size(); ++i) {
        if (TRACE_FILE[i] != '-') {
            RAND_SEED += TRACE_FILE[i];
            printf("%c %d %d\n", TRACE_FILE[i], i, RAND_SEED);
        }
        else
            break;
    }
    srand(RAND_SEED);   // set the seed for C++ rand() based on trace name
    cout << "RAND_SEED: " << RAND_SEED << endl;
    
    // initialize LLC cache
    for (uint32_t set=0; set < LLC_SET; ++set) {
        for (uint32_t way=0; way < LLC_WAY; ++way)
            CACHE[set][way] = 0;
    }

    // initialize global history using provided trace
    init_global_history();   
    init_global_page_table();
    cout << "Global History initialization complete" << endl;
    cout << "Global History size:    " << GLOBAL_HISTORY.size() << endl;
    cout << "Global Page Table size: " << GLOBAL_PAGETABLE.size() << endl;


    if (INDEXING_SCHEME == "GENETIC" || INDEXING_SCHEME == "FIXED_IDX_GENETIC") {
        // initialize indexing
        Index* indexing = init_indexing_scheme();
        GeneticIndex* genetic_index = dynamic_cast<GeneticIndex*>(indexing);

        vector<vector<Translation>> translation_list;
        translation_list.push_back(GLOBAL_PAGETABLE);   // get the original translation

        for (uint32_t gen=0; gen < NUM_GENERATIONS; ++gen) {
            cout << "Generation: " << gen+1 << ". Total population: " << genetic_index->population_size() << ". Total past candidates: " << genetic_index->num_past_candidates() << endl;
            genetic_index->print_population(true);

            if (translation_list.size() == NUM_TRANSLATION_PER_GEN) {
                // even generations, drop oldest (NUM_TRANSLATION_PER_GEN-NUM_RETAINED_SCORE_PER_GEN) of translations
                if (gen % 2 == 0) { 
                    translation_list.erase(translation_list.begin()+1, translation_list.begin()+1+NUM_TRANSLATION_PER_GEN-NUM_RETAINED_SCORE_PER_GEN);
                    cout << "Oldest translations dropped. translation list size: " << translation_list.size() << endl;
                }
                // odd generations, drop newest (NUM_TRANSLATION_PER_GEN-NUM_RETAINED_SCORE_PER_GEN) of translations
                else {  
                    translation_list.resize(NUM_RETAINED_SCORE_PER_GEN); 
                    cout << "Newest translations dropped. translation list size: " << translation_list.size() << endl;
                }

                // drop scores of dropped translations
                genetic_index->drop_scores((gen % 2 == 0));
            }

            // fill translation_list with newly generated random mappings
            for (uint32_t trans=translation_list.size(); trans < NUM_TRANSLATION_PER_GEN; ++trans) {
                reset_va_pa_mapping();
                change_va_pa_mapping();
                translation_list.push_back(GLOBAL_PAGETABLE);
            }

            for (uint32_t trans=0; trans < translation_list.size(); ++trans) {
                cout << "\nTranslation #" << trans+1 << endl;
                GLOBAL_PAGETABLE = translation_list[trans];

                // simulate once for each candidate
                for (uint32_t num_sim=genetic_index->curr_candidate_idx(); num_sim < genetic_index->population_size(); ++num_sim) {
                    cout << "Current Candidate: ";
                    genetic_index->print_candidate(genetic_index->curr_candidate_idx());
                    // Candidate test_candidate = genetic_index->get_candidate(genetic_index->curr_candidate_idx());
                    // cout << test_candidate.to_string() << " [score: " << test_candidate.fitness() << "]" << "[hist_len: " << test_candidate.m_score_hist.size() << "]" << endl;

                    if (genetic_index->get_candidate(genetic_index->curr_candidate_idx()).history_length() > trans) {
                        // cout << "trans: " << trans << ". curr_idx: " << genetic_index->curr_candidate_idx() << ". size: " << test_candidate.history_length() << endl;
                        cout << "This candidate has been evaluated against this translation in a previous generation. Skipping" << endl;
                        genetic_index->curr_candidate_idx(genetic_index->curr_candidate_idx()+1);
                        continue;
                    }

                    // initialize replacement policy
                    Replacement* replacement_policy = init_replacement_policy(indexing);

                    // execute the main loop
                    main_loop(replacement_policy, indexing);
                    genetic_index->next_candidate();

                    delete replacement_policy;
                    reset_statistics();
                }
                genetic_index->curr_candidate_idx(0);   // reset candidate list index ptr
            }

            // evaluate current population: sort candidate, save the current best, cut off underperform candidates
            bool is_emigrating = genetic_index->evaluate();

            if (!is_emigrating)
                cout << "Top " << TOP_CANDIDATE_CUTOFF << " candidates: " << endl;
            else
                cout << "Top candidates leftover: " << endl;

            genetic_index->print_population(true);
            
            // if all top candidates have been removed due to low improvement over XORHASH => 5X the random migration rate
            uint32_t num_migrants = rand() % MAX_MIGRANTS + 1;
            double manual_mutation_rate = INIT_MUTATION_RATE;
            bool mutation_rate_override = false;
            if (genetic_index->population_size() < TOP_CANDIDATE_CUTOFF) {
                num_migrants *= 10;
                manual_mutation_rate = INIT_MUTATION_RATE*1.25;
                if (manual_mutation_rate > 1.0) 
                    manual_mutation_rate = 1.0;
                cout << "Num migrants generated increased due to low population: " << num_migrants << endl;
                cout << "Mutation rate increased due to low population: " << manual_mutation_rate << endl;
            }

            // generate new population with crossover (mutate + reproduce)
            genetic_index->crossover(mutation_rate_override, manual_mutation_rate);
            genetic_index->generate_migrants(num_migrants);
            genetic_index->curr_candidate_idx(0);   // reset candidate list index ptr
            cout << endl;
        }
    }

    else if (INDEXING_SCHEME == "SELECTIVE_GENETIC") {
        // initialize indexing
        Index* indexing = init_indexing_scheme();
        SelectiveGenetic* genetic_index = dynamic_cast<SelectiveGenetic*>(indexing);

        for (uint32_t gen=0; gen < NUM_GENERATIONS; ++gen) {
            cout << "Generation: " << gen+1 << ". Total population: " << genetic_index->population_size() << ". Total past candidates: " << genetic_index->num_past_candidates() << endl;
            genetic_index->print_population();

            // simulate once for each candidate
            for (uint32_t num_sim=genetic_index->curr_candidate_idx(); num_sim < genetic_index->population_size(); ++num_sim) {
                cout << "\nCurrent Candidate: ";
                genetic_index->print_candidate(genetic_index->curr_candidate_idx());
                // initialize replacement policy
                Replacement* replacement_policy = init_replacement_policy(indexing);

                // execute the main loop
                main_loop(replacement_policy, indexing);
                genetic_index->next_candidate();

                delete replacement_policy;
                reset_statistics();
            }

            // evaluate current population: sort candidate, save the current best, cut off underperform candidates
            bool is_emigrating = genetic_index->evaluate();

            // generate new random VA-PA translation
            if (TRANSLATION_ENABLE) {
                srand(RAND_SEED++);
                cout << "RAND_SEED: " << RAND_SEED << endl;
                reset_va_pa_mapping();
                change_va_pa_mapping();
                genetic_index->curr_candidate_idx(0);   // if VA-PA mapping change, we need to re-evaluate all top candidates
                cout << "VA-PA mapping changed" << endl;
            }

            if (!is_emigrating)
                cout << "Top " << TOP_CANDIDATE_CUTOFF << " candidates: " << endl;
            else
                cout << "Top candidates leftover: " << endl;

            genetic_index->print_population(true);

            // generate new population with crossover (mutate + reproduce)
            genetic_index->crossover();
            cout << endl;
        }
    }

    else {
        for (uint32_t num_sim=0; num_sim < NUM_LOOPS; ++num_sim) {
            // initialize indexing
            Index* indexing = init_indexing_scheme();

            // initialize replacement policy
            Replacement* replacement_policy = init_replacement_policy(indexing);

            // load previous checkpoint (if CHECKPOINT is ON)
            if (CHECKPOINT) {
                // open checkpoint file
                string filename;
                for (uint32_t i=0; i < TRACE_FILE.size()-13; ++i)
                    filename += TRACE_FILE[i];
                filename += "CHECKPOINT.txt";
                ifstream cp_file(filename);

                if (cp_file) {
                    load_checkpoint(cp_file);                 // load simulation data and settings 
                    replacement_policy->load_state(cp_file);  // load replacement policy data
                    cout << "Checkpoint loaded successfully" << endl;
                }
                cp_file.close();
            }

            // execute the main loop
            main_loop(replacement_policy, indexing);

            // generate new random VA-PA translation
            if (TRANSLATION_ENABLE) {
                srand(RAND_SEED++);
                cout << "RAND_SEED: " << RAND_SEED << endl;
                reset_va_pa_mapping();
                change_va_pa_mapping();
                cout << "VA-PA mapping changed" << endl;
            }
            
            delete replacement_policy;
            delete indexing;
            reset_statistics();
        }
    }

    return 0;
}