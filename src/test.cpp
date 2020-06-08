#include "catch.hpp"
#include "lib.hpp"

TEST_CASE("correctly formats time") {
  CHECK(formatTime(100) == "00:01:40");
  CHECK(formatTime(1024) == "00:17:04");
  CHECK(formatTime(370800) == "103:00:00");
  CHECK(formatTime(370963) == "103:02:43");
}

TEST_CASE("correctly inputs to ss") {
  std::fstream filestream{"text-for-test", std::ios_base::in};
  std::stringstream ss;
  filestream >> ss;
  CHECK(ss.str() == "line1");
  ss.str("");
  filestream >> ss;
  CHECK(ss.str() == "line2");
  ss.str("");
  filestream >> ss;
  CHECK(ss.str() == "line3");
  ss.str("");
  filestream >> ss;
  CHECK(ss.str() == "line4");
  ss.str("");
  filestream >> ss;
  CHECK(ss.str() == "");
  filestream.close();
}

TEST_CASE("correctly inputs to pair") {
  std::fstream filestream{"text-for-test", std::ios_base::in};
  std::pair<std::stringstream, std::stringstream> pair;
  REQUIRE(filestream >> pair);
  CHECK(pair.first.str() == "line1");
  CHECK(pair.second.str() == "line2");
  pair.first.str("");
  pair.second.str("");
  REQUIRE(filestream >> pair);
  CHECK(pair.first.str() == "line3");
  CHECK(pair.second.str() == "line4");
  pair.first.str("");
  pair.second.str("");
  CHECK(!(filestream >> pair));
  filestream.close();
}

#define MUSIC_INIT                                                             \
  nana::form fm;                                                               \
  sf::Music music;                                                             \
  nana::slider seeker{fm, true};                                               \
  nana::label lab{fm, ""};                                                     \
  nana::button play_pause{fm, ""}

TEST_CASE("correctly opens existing file") {
  MUSIC_INIT;
  std::pair<std::string, std::string> filenamepath{"some_sound", "./sound.wav"};
  bool play_next = 0;

  openPlay(music, filenamepath, seeker, lab, play_pause, play_next);
  REQUIRE(music.getStatus() == sf::Music::Playing);
  CHECK(play_next == 0);
  CHECK(lab.caption() == "Playing: <bold>" + filenamepath.first + "</>");
  CHECK(play_pause.caption() == "Play");
  CHECK(seeker.maximum() == music.getDuration().asMilliseconds());
}

TEST_CASE("skips invalid file") {
  MUSIC_INIT;
  std::pair<std::string, std::string> filenamepath{"some_invelid_sound",
                                                   "./sound1.wav"};
  bool play_next = 0;

  openPlay(music, filenamepath, seeker, lab, play_pause, play_next);
  REQUIRE(music.getStatus() == sf::Music::Stopped);
  CHECK(play_next == 1);
  CHECK(lab.caption() ==
        "<bold>Error! File not found or format is incorrect.</>");
  CHECK(play_pause.caption() == "Stop");
}