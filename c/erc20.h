#ifndef ERC20_H_
#define ERC20_H_

#define ADDRESS_LENGTH 32

#define MAX_BALANCES 100
#define MAX_ALLOWED 100

#define ERROR_NO_BALANCE_FOUND -21
#define ERROR_NO_BALANCE_SPACE -22
#define ERROR_NOT_SUFFICENT_BALANCE -23
#define ERROR_OVERFLOW -24
#define ERROR_NO_ALLOWED_FOUND -25
#define ERROR_NO_ALLOWED_SPACE -26
#define ERROR_NOT_SUFFICENT_ALLOWED -27

typedef struct {
  char address[ADDRESS_LENGTH + 1];
  int64_t tokens;
} balance_t;

typedef struct {
  char address[ADDRESS_LENGTH + 1];
  char spender[ADDRESS_LENGTH + 1];
  int64_t tokens;
} allowed_t;

typedef struct {
  balance_t balances[MAX_BALANCES];
  int used_balance;
  allowed_t allowed[MAX_ALLOWED];
  int used_allowed;

  char owner[ADDRESS_LENGTH + 1];
  char newOwner[ADDRESS_LENGTH + 1];
  int64_t total_supply;
} data_t;

int erc20_initialize(data_t *data, const char owner[ADDRESS_LENGTH], int64_t total_supply)
{
  strncpy(data->owner, owner, ADDRESS_LENGTH);
  strncpy(data->balances[0].address, owner, ADDRESS_LENGTH);
  memset(data->newOwner, 0, ADDRESS_LENGTH);

  data->balances[0].tokens = total_supply;
  data->used_balance = 1;
  data->used_allowed = 0;
  data->total_supply = total_supply;
}

int erc20_total_supply(const data_t *data)
{
  return data->total_supply;
}

int _erc20_find_balance(data_t *data, const char address[ADDRESS_LENGTH], int ensure, balance_t **balance)
{
  int i = 0, cmp = -1;
  for (; i < data->used_balance; i++) {
    cmp = strncmp(data->balances[i].address, address, ADDRESS_LENGTH);
    if (cmp >= 0) {
      break;
    }
  }
  if (cmp == 0) {
    *balance = &(data->balances[i]);
    return 0;
  }
  if (!ensure) {
    return ERROR_NO_BALANCE_FOUND;
  }
  int j = data->used_balance + 1;
  if (j > MAX_BALANCES) {
    return ERROR_NO_BALANCE_SPACE;
  }
  for (; j > i; j--) {
    memcpy(&(data->balances[j]), &(data->balances[j - 1]), sizeof(balance_t));
  }
  strncpy(data->balances[i].address, address, ADDRESS_LENGTH);
  data->balances[i].tokens = 0;
  data->used_balance++;
  *balance = &(data->balances[i]);
  return 0;
}

int64_t erc20_balance_of(data_t *data, const char address[ADDRESS_LENGTH])
{
  balance_t *balance = NULL;
  int ret = _erc20_find_balance(data, address, 0, &balance);
  if (ret != 0) {
    return ret;
  }
  return balance->tokens;
}

int erc20_transfer(data_t *data, const char from[ADDRESS_LENGTH], const char to[ADDRESS_LENGTH], int64_t tokens)
{
  balance_t *from_balance = NULL, *to_balance = NULL;
  int ret = _erc20_find_balance(data, from, 1, &from_balance);
  if (ret != 0) {
    return ret;
  }
  ret = _erc20_find_balance(data, to, 1, &to_balance);
  if (ret != 0) {
    return ret;
  }
  if (from_balance->tokens < tokens) {
    return ERROR_NOT_SUFFICENT_BALANCE;
  }
  int target = to_balance->tokens + tokens;
  if (target < to_balance->tokens) {
    return ERROR_OVERFLOW;
  }
  from_balance->tokens -= tokens;
  to_balance->tokens = target;
  return 0;
}

int _erc20_find_allowed(data_t *data, const char address[ADDRESS_LENGTH], const char spender[ADDRESS_LENGTH], int ensure, allowed_t **allowed)
{
  int i = 0, cmp = -1;
  for (; i < data->used_allowed; i++) {
    cmp = strncmp(data->allowed[i].address, address, ADDRESS_LENGTH);
    if (cmp == 0) {
      cmp = strncmp(data->allowed[i].spender, spender, ADDRESS_LENGTH);
    }
    if (cmp >= 0) {
      break;
    }
  }
  if (cmp == 0) {
    *allowed = &(data->allowed[i]);
    return 0;
  }
  if (!ensure) {
    return ERROR_NO_ALLOWED_FOUND;
  }
  int j = data->used_allowed + 1;
  if (j > MAX_ALLOWED) {
    return ERROR_NO_ALLOWED_SPACE;
  }
  for (; j > i; j--) {
    memcpy(&(data->allowed[j]), &(data->allowed[j - 1]), sizeof(allowed_t));
  }
  strncpy(data->allowed[i].address, address, ADDRESS_LENGTH);
  strncpy(data->allowed[i].spender, spender, ADDRESS_LENGTH);
  data->allowed[i].tokens = 0;
  data->used_allowed++;
  *allowed = &(data->allowed[i]);
  return 0;
}

int erc20_approve(data_t *data, const char from[ADDRESS_LENGTH], const char spender[ADDRESS_LENGTH], int64_t tokens)
{
  allowed_t *allowed = NULL;
  int ret = _erc20_find_allowed(data, from, spender, 1, &allowed);
  if (ret != 0) {
    return ret;
  }
  allowed->tokens = tokens;
  return 0;
}

int erc20_transfer_from(data_t *data, const char from[ADDRESS_LENGTH], const char spender[ADDRESS_LENGTH], const char to[ADDRESS_LENGTH], int64_t tokens)
{
  balance_t *from_balance = NULL, *to_balance = NULL;
  allowed_t *allowed = NULL;
  int ret = _erc20_find_balance(data, from, 1, &from_balance);
  if (ret != 0) {
    return ret;
  }
  ret = _erc20_find_balance(data, to, 1, &to_balance);
  if (ret != 0) {
    return ret;
  }
  ret = _erc20_find_allowed(data, from, spender, 1, &allowed);
  if (ret != 0) {
    return ret;
  }
  if (from_balance->tokens < tokens) {
    return ERROR_NOT_SUFFICENT_BALANCE;
  }
  if (allowed->tokens < tokens) {
    return ERROR_NOT_SUFFICENT_ALLOWED;
  }
  int target = to_balance->tokens + tokens;
  if (target < to_balance->tokens) {
    return ERROR_OVERFLOW;
  }
  from_balance->tokens -= tokens;
  allowed->tokens -= tokens;
  to_balance->tokens = target;
  return 0;
}

int64_t erc20_allowance(data_t *data, const char from[ADDRESS_LENGTH], const char spender[ADDRESS_LENGTH])
{
  allowed_t *allowed = NULL;
  int ret = _erc20_find_allowed(data, from, spender, 0, &allowed);
  if (ret != 0) {
    return ret;
  }
  return allowed->tokens;
}

void __erc20_debug_print(const data_t *data)
{
  printf("Owner %s\n", data->owner);
  printf("New Owner %s\n", data->newOwner);
  printf("Total supply: %ld\n", data->total_supply);
  printf("Balance Count: %d\n", data->used_balance);
  for (int i = 0; i < data->used_balance; i++) {
    printf("  Address: %s tokens: %ld\n", data->balances[i].address,
           data->balances[i].tokens);
  }
  printf("Allowed Count: %d\n", data->used_allowed);
  for (int i = 0; i < data->used_allowed; i++) {
    printf("  Address: %s Spender: %s tokens: %ld\n", data->allowed[i].address,
           data->allowed[i].spender, data->allowed[i].tokens);
  }
}

#endif
