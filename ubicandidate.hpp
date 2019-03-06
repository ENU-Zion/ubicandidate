#include <enulib/time.hpp>
#include <enulib/asset.hpp>
#include <enulib/singleton.hpp>
#include <enulib/transaction.hpp>
//#include <enulib/crypto.h>
#include "enu.token.hpp"

using namespace enumivo;
using namespace std;

#define ADMIN N(qsx.io)

class ubicandidate : public contract
{
public:
  ubicandidate(account_name self)
      : contract(self),
        _member(_self, _self),
        _candidate(_self, _self),
        _global(_self, _self){};

  // @abi action
  void add(const account_name &user);

  // @abi action
  void apply(const account_name &user);

  // @abi action
  void activate(const account_name &user);

  // @abi action
  void vote(const account_name &voter, const account_name &applicant, const bool opinion);

  // @abi action
  void reset();

private:
  void init()
  {
    _global.remove();
    global_state _gstate;
    _gstate.member_num = 0;
    _gstate.candidate_num = 0;
    _global.set(_gstate, _self);
  }

  void add_member(const account_name &user)
  {
    enumivo_assert(_member.find(user) == _member.end(), "member exists");

    //del candidate
    auto itr = _candidate.find(user);
    enumivo_assert(itr != _candidate.end(), "candidate not found");
    _candidate.erase(itr);

    _member.emplace(_self, [&](auto &m) {
      m.name = user;
    });

    //add number to global
    enumivo_assert(_global.exists(), "global not exists");
    global_state _gstate;
    _gstate = _global.get();
    _gstate.member_num = _gstate.member_num + 1;
    enumivo_assert(_gstate.candidate_num > 0, "candidate_num should >0");
    _gstate.candidate_num = _gstate.candidate_num - 1;
    _global.set(_gstate, _self);
  }

  void add_candidate(const account_name &user)
  {
    enumivo_assert(_candidate.find(user) == _candidate.end(), "candidate exists");
    enumivo_assert(_member.find(user) == _member.end(), "member exists");

    _candidate.emplace(_self, [&](auto &c) {
      c.name = user;
      c.apply_time = now();
    });

    //add number to global
    enumivo_assert(_global.exists(), "global not exists");
    global_state _gstate;
    _gstate = _global.get();
    _gstate.candidate_num = _gstate.candidate_num + 1;
    _global.set(_gstate, _self);
  }

  bool is_active()
  {
    enumivo_assert(_global.exists(), "global not exists");
    global_state _gstate;
    _gstate = _global.get();
    return _gstate.member_num > 1000;
  }

  /* uint64_t _random(account_name user, uint64_t range)
  {
    auto mixd = tapos_block_prefix() * tapos_block_num() + user + current_time();
    const char *mixedChar = reinterpret_cast<const char *>(&mixd);
    checksum256 result;
    sha256((char *)mixedChar, sizeof(mixedChar), &result);
    uint64_t num1 = *(uint64_t *)(&result.hash[0]) + *(uint64_t *)(&result.hash[8]) * 10 + *(uint64_t *)(&result.hash[16]) * 100 + *(uint64_t *)(&result.hash[24]) * 1000;
    uint64_t random_num = (num1 % range);
    return random_num;
  } */

  // @abi table member i64
  struct member_info
  {
    account_name name;
    uint64_t last_claim_time = 0;
    uint64_t primary_key() const { return name; }
    ENULIB_SERIALIZE(member_info, (name)(last_claim_time))
  };

  typedef enumivo::multi_index<N(member), member_info> member_index;
  member_index _member;

  // @abi table candidate i64
  struct candidate
  {
    account_name name;
    uint64_t apply_time = 0;
    vector<account_name> yes_list;
    vector<account_name> no_list;
    uint64_t primary_key() const { return name; }
    ENULIB_SERIALIZE(candidate, (name)(apply_time)(yes_list)(no_list))
  };

  typedef enumivo::multi_index<N(candidate), candidate> candidate_index;
  candidate_index _candidate;

  // @abi table global i64
  struct global_state
  {
    uint64_t member_num = 0;
    uint64_t candidate_num = 0;
    ENULIB_SERIALIZE(global_state, (member_num)(candidate_num))
  };
  typedef enumivo::singleton<N(global), global_state> global_state_singleton;
  global_state_singleton _global;
};

ENUMIVO_ABI(ubicandidate, (add)(reset)(apply)(vote)(activate))