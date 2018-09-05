#include "ckb.h"

#include "erc20_reader.h"
#include "erc20.h"

#undef ns
#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(ERC20_Sample, x)

int parse_data(const char* buf, size_t len, data_t *out)
{
  (void) len;

  ns(Data_table_t) data;
  if (!(data = ns(Data_as_root(buf)))) {
    return -1;
  }
  const char *owner = ns(Data_owner(data));
  uint64_t total_supply = ns(Data_total_supply(data));

  erc20_initialize(out, owner, total_supply);

  ns(Balance_vec_t) balances = ns(Data_balances(data));
  size_t balance_size = ns(Balance_vec_len(balances));
  for (size_t i = 0; i < balance_size; i++) {
    const char *address = ns(Balance_address(ns(Balance_vec_at(balances, i))));
    uint64_t tokens = ns(Balance_tokens(ns(Balance_vec_at(balances, i))));
    balance_t *balance_out = NULL;

    int ret = _erc20_find_balance(out, address, 0, &balance_out);
    if (ret != ERROR_NO_BALANCE_FOUND) {
      /* Duplicate entry is found */
      return -1;
    }
    ret = _erc20_find_balance(out, address, 1, &balance_out);
    if (ret != 0) {
      return ret;
    }
    balance_out->tokens = tokens;
  }

  ns(Allowed_vec_t) allowed = ns(Data_allowed(data));
  size_t allowed_size = ns(Allowed_vec_len(allowed));
  for (size_t i = 0; i < allowed_size; i++) {
    const char *address = ns(Allowed_address(ns(Allowed_vec_at(balances, i))));
    const char *spender = ns(Allowed_spender(ns(Allowed_vec_at(balances, i))));
    uint64_t tokens = ns(Allowed_tokens(ns(Allowed_vec_at(balances, i))));
    allowed_t *allowed_out = NULL;

    int ret = _erc20_find_allowed(out, address, spender, 0, &allowed_out);
    if (ret != ERROR_NO_ALLOWED_FOUND) {
      /* Duplicate entry is found */
      return -1;
    }
    ret = _erc20_find_allowed(out, address, spender, 1, &allowed_out);
    if (ret != 0) {
      return ret;
    }
    allowed_out->tokens = tokens;
  }

  return 0;
}

#define MAX_CELL_NUMBER 10
static int deps_count = 0;
static char* deps_cells[MAX_CELL_NUMBER];
static int input_count = 0;
static char* input_cells[MAX_CELL_NUMBER];
static int output_count = 0;
static char* output_cells[MAX_CELL_NUMBER];

int parse_args(int argc, char* argv[])
{
  if (argc < 4) {
    return -1;
  }
  int i = 1;

  deps_count = atoi(argv[i++]);
  if (deps_count < 0 || deps_count > MAX_CELL_NUMBER) {
    return -2;
  }
  if (argc < i + deps_count + 2) {
    return -1;
  }
  for (int j = 0; j < deps_count; j++) {
    deps_cells[j] = argv[i + j];
  }
  i += deps_count;

  input_count = atoi(argv[i++]);
  if (input_count < 0 || input_count > MAX_CELL_NUMBER) {
    return -2;
  }
  if (argc < i + input_count + 1) {
    return -1;
  }
  for (int j = 0; j < input_count; j++) {
    input_cells[j] = argv[i + j];
  }
  i += input_count;

  output_count = atoi(argv[i++]);
  if (output_count < 0 || output_count > MAX_CELL_NUMBER) {
    return -2;
  }
  if (argc < i + output_count) {
    return -1;
  }
  for (int j = 0; j < output_count; j++) {
    output_cells[j] = argv[i + j];
  }
  return 0;
}

#define MAX_COMMAND_LENGTH 256
#define MAX_CELL_LENGTH 1048576
#define CHECK_RET(x) ret = (x); if (ret != 0) { return ret; }

static char commands[4][MAX_COMMAND_LENGTH];
static char input_cell_data[MAX_CELL_LENGTH];
static char output_cell_data[MAX_CELL_LENGTH];
static data_t input_cell;
static data_t output_cell;

int main(int argc, char* argv[])
{
  int ret = -1;
  CHECK_RET(parse_args(argc, argv));

  /*
   * In current IO group, only one input cell and one output cell are permitted.
   */
  if (input_count != 1 || output_count != 1) {
    return -3;
  }

  int input_argc = 0;
  CHECK_RET(ckb_input_cell_argc(input_cells[0], &input_argc));

  /*
   * 1 command name, 1 pubkey, 1 signature, 1 command, at least we are looking at
   * input_argc of 4, validation script accepts 4 arguments at most, so input_argc is
   * 7 at maximum
   */
  if (input_argc < 4 || input_argc > 7) {
    return -4;
  }
  for (int i = 0; i < input_argc - 3; i++) {
    size_t len = 256;
    CHECK_RET(ckb_input_cell_argv(input_cells[0], i + 3, commands[i], &len));
  }

  /*
   * Parse input & output cells
   */
  size_t input_cell_size = MAX_CELL_LENGTH;
  CHECK_RET(ckb_mmap_cell(input_cells[0], input_cell_data, &input_cell_size));
  CHECK_RET(parse_data(input_cell_data, input_cell_size, &input_cell));
  size_t output_cell_size = MAX_CELL_LENGTH;
  CHECK_RET(ckb_mmap_cell(output_cells[0], output_cell_data, &output_cell_size));
  CHECK_RET(parse_data(output_cell_data, output_cell_size, &output_cell));

  if (memcmp(commands[0], "transfer", 8) == 0) {
    CHECK_RET(erc20_transfer(&input_cell, commands[1], commands[2], atoll(commands[3])));
    if (memcmp(&input_cell, &output_cell, sizeof(input_cell)) == 0) {
      return 0;
    } else {
      return -6;
    }
  } else {
    /* Unknown command */
    return -5;
  }

  return 0;
}
