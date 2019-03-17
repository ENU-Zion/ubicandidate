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
  auto itr3 = _vote.begin();
  while (itr3 != _vote.end())
  {
    _vote.erase(itr3);
    itr3 = _vote.begin();
  }
  _global.remove();
}

void ubicandidate::add(const account_name &user)
{
  require_auth(ADMIN);
  //for test
  //enumivo_assert(!is_active(), "ubi community have activated");

  add_member(user);
}

void ubicandidate::remove(const account_name &user)
{
  //for test
  require_auth(ADMIN);

  remove_member(user);
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
  txn.delay_sec = APPLICATION_WAIT_TIME + 60;
  txn.send(get_next_id(), _self, false);
}

void ubicandidate::vote(const account_name &voter, const account_name &candidate, const bool opinion, const string vote_comment)
{
  require_auth(voter);

  //fot test
  //enumivo_assert(is_active(), "ubi community have not activated");

  //check voter in member
  _member.get(voter, "you are not member, can not vote!");

  auto candidate_itr = _candidate.find(candidate);
  enumivo_assert(candidate_itr != _candidate.end(), "candidate not found");

  enumivo_assert(candidate_itr->close_time == 0, "applicaion is closed");

  //delete exist vote
  auto yes_list = candidate_itr->yes_list;
  auto yes_itr = std::find(yes_list.begin(), yes_list.end(), voter);
  if (yes_itr != yes_list.end())
  {
    yes_list.erase(yes_itr);
  }

  auto no_list = candidate_itr->no_list;
  auto no_itr = std::find(no_list.begin(), no_list.end(), voter);
  if (no_itr != no_list.end())
  {
    no_list.erase(no_itr);
  }

  if (opinion)
  {
    yes_list.push_back(voter);
  }
  else
  {
    no_list.push_back(voter);
  }
  _candidate.modify(candidate_itr, 0, [&](auto &c) {
    c.yes_list = yes_list;
    c.no_list = no_list;
  });

  //update vote
  auto vote_id = combine_ids(voter, candidate);
  auto record_index = _vote.get_index<N(get_record)>();
  auto vote_itr = record_index.find(vote_id);
  if (vote_itr == record_index.end())
  {
    _vote.emplace(voter, [&](auto &c) {
      c.id = get_next_id();
      c.voter = voter;
      c.candidate = candidate;
      c.opinion = opinion;
      c.content = vote_comment;
      c.vote_time = now();
    });
  }
  else
  {
    record_index.modify(vote_itr, 0, [&](auto &c) {
      c.opinion = opinion;
      c.content = vote_comment;
      c.vote_time = now();
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