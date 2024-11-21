#include "ooo_cpu.h"

class OptRepl {
private:
    vector<uint64_t> set_history[NUM_CPUS][LLC_SET];
    // vector<Access> set_history[NUM_CPUS][LLC_SET];
    uint64_t exec_tracker[LLC_SET];
public:
    OptRepl() {}
    void initialize(uint32_t cpu, string trace_file);
    void update(uint32_t cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit);
    uint32_t get_victim(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type);
    uint64_t convert_addr(uint32_t cpu, uint64_t ip, uint64_t full_addr, uint64_t inst_id);
};