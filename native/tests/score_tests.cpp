#include "skifree/scores.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
int checks = 0;
void check(bool value, const std::string& description) {
  if (!value) throw std::runtime_error(description);
  ++checks;
  std::cout << "PASS: " << description << '\n';
}
}

int main() {
  try {
    using ski::scores::submit;
    std::vector<int> table;
    auto result = submit(table, 12340, true);
    check(table == std::vector<int>{12340} && result.rank == 1 && result.value == 12340 && result.timed,
          "First timed result creates exactly one real score");
    check(result.isCurrentRow(0) && !result.isCurrentRow(1),
          "First-place result identifies only its own display row");

    table = {9000, 10000, 11000};
    result = submit(table, 9500, true);
    check(table == std::vector<int>({9000, 9500, 10000, 11000}) && result.rank == 2,
          "Timed results rank shorter elapsed times first");
    table = {9000, 10000, 10000, 11000};
    result = submit(table, 10000, true);
    check(table == std::vector<int>({9000, 10000, 10000, 10000, 11000}) && result.rank == 4,
          "New timed tie follows all existing equal times");
    check(!result.isCurrentRow(1) && !result.isCurrentRow(2) && result.isCurrentRow(3),
          "Tie marker identifies the submitted result rather than an older equal score");

    table = {300, 200, 100};
    result = submit(table, 250, false);
    check(table == std::vector<int>({300, 250, 200, 100}) && result.rank == 2 && !result.timed,
          "Freestyle results rank higher scores first");
    table = {300, 200, 200, 100};
    result = submit(table, 200, false);
    check(table == std::vector<int>({300, 200, 200, 200, 100}) && result.rank == 4,
          "New freestyle tie follows all existing equal scores");

    table = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};
    result = submit(table, 9500, true);
    check(table.back() == 9500 && table.size() == 10 && result.rank == 10,
          "Qualifying tenth-place time replaces only the previous last result");
    const auto fullTimes = table;
    result = submit(table, 9500, true);
    check(table == fullTimes && !result.qualified() && result.value == 9500 && result.timed,
          "Tie with tenth place is rejected but retained for the try-again display");
    check(!result.isCurrentRow(0) && !result.isCurrentRow(9),
          "Nonqualifying result marks no existing high-score row");
    result = submit(table, 20000, true);
    check(table == fullTimes && result.rank == 0 && result.value == 20000,
          "Slower nonqualifying time is still available outside the top-ten table");

    table = {100, 90, 80, 70, 60, 50, 40, 30, 20, 10};
    result = submit(table, 95, false);
    check(table == std::vector<int>({100, 95, 90, 80, 70, 60, 50, 40, 30, 20}) && result.rank == 2,
          "Freestyle insertion preserves order and drops the displaced eleventh score");
    const auto fullScores = table;
    result = submit(table, -5, false);
    check(table == fullScores && result.rank == 0 && result.value == -5 && !result.timed,
          "Low freestyle result is kept for feedback without changing stored scores");

    table.clear();
    result = submit(table, -10, false);
    check(table == std::vector<int>{-10} && result.rank == 1,
          "Negative freestyle results remain valid when the table has room");

    table.assign(11, 42);
    bool rejected = false;
    try { submit(table, 1, true); }
    catch (const std::invalid_argument&) { rejected = true; }
    check(rejected && table == std::vector<int>(11, 42),
          "Oversized adapter input is rejected before mutation");
    using ski::scores::Entry;
    std::vector<Entry> named{{1000,"ONE"},{1000,"TWO"},{2000,"OLD"}};
    result=submit(named,1000,true);
    check(result.rank==3&&named==std::vector<Entry>{{1000,"ONE"},{1000,"TWO"},{1000,"AAA"},{2000,"OLD"}},
          "Named ties preserve existing identities and place new initials on their own row");
    named.clear();for(int i=1;i<=10;++i)named.emplace_back(i*100,"OLD");
    const auto unchanged=named;result=submit(named,2000,true);
    check(!result.qualified()&&named==unchanged,"Nonqualifying named result changes no initials");
    result=submit(named,950,true);
    check(result.rank==10&&named.size()==10&&named.back()==Entry{950,"AAA"},"Named tenth-place replacement preserves limit");
    named={{300,"TOP"},{100,"LOW"}};result=submit(named,200,false);
    check(result.rank==2&&named[0].initials=="TOP"&&named[1]==Entry{200,"AAA"}&&named[2].initials=="LOW","Freestyle initials stay attached to descending scores");
    std::cout << checks << " score behavior checks passed\n";
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "FAIL: " << error.what() << '\n';
    return 1;
  }
}
