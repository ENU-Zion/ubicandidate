#include <enulib/time.hpp>
#include <enulib/asset.hpp>
#include <enulib/singleton.hpp>
#include <enulib/transaction.hpp>
//#include <enulib/crypto.h>
#include "enu.token.hpp"

using namespace enumivo;
using namespace std;

#define ADMIN N(qsx.io)
#define ACTIVE_THRESHOLD 10
#define APPLICATION_WAIT_TIME (uint64_t(3) * 24 * 60 * 60)
#define RETRY_WAIT_TIME (uint64_t(1) * 24 * 60 * 60)
#define CLAIM_WAIT_TIME (uint64_t(1) * 24 * 60 * 60)
#define VOTE_RATE 50
#define WIN_RATE 66
#define NO_REWARD_RATE 98

class ubicandidate : public contract
{
public:
  ubicandidate(account_name self)
      : contract(self),
        _member(_self, _self),
        _vote(_self, _self),
        _candidate(_self, _self),
        _global(_self, _self){};

  // @abi action
  void add(const account_name &user);

  // @abi action
  void apply(const account_name &user);

  // @abi action
  void activate(const account_name &user);

  // @abi action
  void claim(const account_name &user);

  // @abi action
  void vote(const account_name &voter, const account_name &candidate, const bool opinion, const string vote_comment);

  // @abi action
  void reset();

  // @abi action
  void init()
  {
    global_state _gstate;
    _gstate.global_id = 0;
    _gstate.member_num = 0;
    _gstate.candidate_num = 0;
    _global.set(_gstate, _self);
  }

  // @abi action
  void remove(const account_name &user);

private:
  uint64_t get_next_id()
  {
    enumivo_assert(_global.exists(), "global not exists");
    global_state _gstate;
    _gstate = _global.get();
    auto next_id = _gstate.global_id + 1;
    _gstate.global_id = next_id;
    _global.set(_gstate, _self);
    return next_id;
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

  void remove_member(const account_name &user)
  {
    //del member
    auto itr = _member.find(user);
    enumivo_assert(itr != _member.end(), "member not found");
    _member.erase(itr);

    //add number to global
    enumivo_assert(_global.exists(), "global not exists");
    global_state _gstate;
    _gstate = _global.get();
    enumivo_assert(_gstate.member_num > 0, "candidate_num should >0");
    _gstate.member_num = _gstate.member_num - 1;
    _global.set(_gstate, _self);
  }

  void claim_reward(const account_name &user)
  {
    auto member_itr = _member.find(user);
    enumivo_assert(member_itr != _member.end(), "member not exists");
    enumivo_assert((now() - member_itr->last_claim_time) > CLAIM_WAIT_TIME, "claim once per 24 hours");

    _member.modify(member_itr, 0, [&](auto &m) {
      m.last_claim_time = now();
    });

    //todo:transfer ubi

    action(permission_level{_self, N(active)}, N(enu.token), N(transfer),
           std::make_tuple(_self, user, asset(10000, S(4, ENU)), std::string("reward of UBI")))
        .send();
  }

  void add_candidate(const account_name &user)
  {
    enumivo_assert(_member.find(user) == _member.end(), "member exists");
    //enumivo_assert(_candidate.find(user) == _candidate.end(), "candidate exists");
    auto candidate_itr = _candidate.find(user);
    if (candidate_itr != _candidate.end())
    {
      enumivo_assert(now() - candidate_itr->close_time > RETRY_WAIT_TIME, "can not apply again in 30days");
      _candidate.erase(candidate_itr);
    }
    else
    {
      //add number to global
      enumivo_assert(_global.exists(), "global not exists");
      global_state _gstate;
      _gstate = _global.get();
      _gstate.candidate_num = _gstate.candidate_num + 1;
      _global.set(_gstate, _self);
    }

    _candidate.emplace(_self, [&](auto &c) {
      c.name = user;
      c.apply_time = now();
    });
  }

  /*
  check is community active
  */
  bool is_active()
  {
    enumivo_assert(_global.exists(), "global not exists");
    global_state _gstate;
    _gstate = _global.get();
    return _gstate.member_num >= ACTIVE_THRESHOLD;
  }

  /* check application result */
  void check_result(const account_name &user)
  {
    const auto &candidate = _candidate.get(user, "you have not apply!");
    global_state _gstate = _global.get();
    auto member_num = _gstate.member_num;
    auto yes_num = candidate.yes_list.size();
    auto no_num = candidate.no_list.size();
    auto voter_num = yes_num + no_num;

    //ubi community have activated
    if (!is_active())
    {
      return;
    }

    if (yes_num * 100 / member_num >= WIN_RATE)
    {
      //get enough vote , do not need wait 30 days.
      close_application(user, true, true);
    }
    //time to close application
    else if ((now() - candidate.apply_time) > APPLICATION_WAIT_TIME)
    {
      // voter number enough
      if (voter_num * 100 / member_num >= VOTE_RATE)
      {
        //to be decide
        //auto reward_flag = (yes_num  *100/ voter_num >= NO_REWARD_RATE || no_num*100 / voter_num  >= NO_REWARD_RATE) ? false : true;
        auto reward_flag = true;

        if (yes_num * 100 / voter_num >= WIN_RATE)
        {
          close_application(user, true, reward_flag);
        }
        else if (no_num * 100 / voter_num >= WIN_RATE)
        {
          close_application(user, false, reward_flag);
        }
        else
        {
          //TODO : if this occasion should reward?
          close_application(user, false, false);
        }
      }
      else
      {
        close_application(user, false, false);
      }
    }
    else
    {
      enumivo_assert(0, "voting is going on");
    }
  }

  void close_application(const account_name &user, bool pass_flag, bool reward_flag)
  {
    auto candidate_itr = _candidate.find(user);

    //give reward
    //TODO is simple logic now
    if (reward_flag)
    {
      auto reward_list = pass_flag ? candidate_itr->yes_list : candidate_itr->no_list;

      for (auto voter : reward_list)
      {
        action(permission_level{_self, N(active)}, N(enu.token), N(transfer),
               std::make_tuple(_self, voter, asset(10000, S(4, ENU)), std::string("reward of UBI community voting of candidate:") + (name{user}).to_string()))
            .send();
      }
    }

    //change table data
    if (pass_flag)
    {
      add_member(user);
    }
    else
    {
      _candidate.modify(candidate_itr, 0, [&](auto &c) {
        c.close_time = now();
      });
    }
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

  // concatenation of ids
  static uint128_t combine_ids(const uint64_t &x, const uint64_t &y)
  {
    return (uint128_t{x} << 64) | y;
  }

  // @abi table vote i64
  struct vote_record
  {
    uint64_t id;
    account_name voter;
    account_name candidate;
    bool opinion;
    string content;
    uint64_t vote_time;
    uint64_t primary_key() const { return id; }
    uint64_t get_by_candidate() const { return candidate; }
    uint128_t get_record() const
    {
      print(combine_ids(voter, candidate));
      return combine_ids(voter, candidate);
    }
    ENULIB_SERIALIZE(vote_record, (id)(voter)(candidate)(opinion)(content)(vote_time))
  };

  /* typedef enumivo::multi_index<N(vote), vote_record> vote_index;
  vote_index _vote; */
  typedef multi_index<N(vote), vote_record, indexed_by<N(get_by_candidate), const_mem_fun<vote_record, uint64_t, &vote_record::get_by_candidate>>,
                      indexed_by<N(get_record), const_mem_fun<vote_record, uint128_t, &vote_record::get_record>>>
      vote_index;
  vote_index _vote;

  // @abi table candidate i64
  struct candidate
  {
    account_name name;
    uint64_t apply_time = 0;
    uint64_t close_time = 0;
    vector<account_name> yes_list;
    vector<account_name> no_list;
    uint64_t primary_key() const { return name; }
    ENULIB_SERIALIZE(candidate, (name)(apply_time)(close_time)(yes_list)(no_list))
  };

  typedef enumivo::multi_index<N(candidate), candidate> candidate_index;
  candidate_index _candidate;

  // @abi table global i64
  struct global_state
  {
    uint64_t global_id = 0;
    uint64_t member_num = 0;
    uint64_t candidate_num = 0;
    ENULIB_SERIALIZE(global_state, (global_id)(member_num)(candidate_num))
  };
  typedef enumivo::singleton<N(global), global_state> global_state_singleton;
  global_state_singleton _global;
};

ENUMIVO_ABI(ubicandidate, (init)(add)(reset)(apply)(vote)(activate)(claim)(remove))