#include "ubicandidate.hpp"

void ubicandidate::reset()
{
  require_auth(_self);
  auto itr = _member.begin();
  while (itr != _member.end())
  {
    _member.erase(itr);
    itr = _member.begin();
  }
  auto itr2 = _candidate.begin();
  while (itr2 != _candidate.end())
  {
    _candidate.erase(itr2);
    itr2 = _candidate.begin();
  }
  ubicandidate::init();
}

void ubicandidate::add(const account_name &user)
{
  require_auth(ADMIN);
  enumivo_assert(!is_active(), "ubi community have activated");

  add_member(user);
}

void ubicandidate::apply(const account_name &user)
{
  require_auth(user);
  add_candidate(user);

  enumivo::transaction txn{};
  txn.actions.emplace_back(
      enumivo::permission_level(_self, N(active)),
      _self,
      N(activate),
      std::make_tuple(user));
  txn.delay_sec = APPLICATION_WAIT_TIME / 1000 + 60;
  txn.send(now(), _self, false);
}

void ubicandidate::vote(const account_name &voter, const account_name &applicant, const bool opinion)
{
  require_auth(voter);
  
  enumivo_assert(is_active(), "ubi community have not activated");

  //check voter in member
  _member.get(voter, "you are not member, can not vote!");

  auto candidate_itr = _candidate.find(applicant);
  enumivo_assert(candidate_itr != _candidate.end(), "applicant not found");

  enumivo_assert(candidate_itr->close_time == 0, "applicaion is closed");

  auto exist = std::count(candidate_itr->yes_list.begin(), candidate_itr->yes_list.end(), voter) +
               std::count(candidate_itr->no_list.begin(), candidate_itr->no_list.end(), voter);

  enumivo_assert(exist == 0, "already voted");

  //TODO:change vote

  if (opinion)
  {
    auto list = candidate_itr->yes_list;
    list.push_back(voter);
    _candidate.modify(candidate_itr, 0, [&](auto &c) {
      c.yes_list = list;
    });
  }
  else
  {
    auto list = candidate_itr->no_list;
    list.push_back(voter);
    _candidate.modify(candidate_itr, 0, [&](auto &c) {
      c.no_list = list;
    });
  }
}

void ubicandidate::activate(const account_name &user)
{
  check_result(user);
}

void ubicandidate::claim(const account_name &user)
{
  claim_reward(user);
}