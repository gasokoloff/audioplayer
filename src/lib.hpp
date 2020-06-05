#pragma once

#include <chrono>
#include <filesystem>
//#include <filesystem>

using namespace std::literals::chrono_literals;
namespace fs = std::filesystem;
//namespace fs = std::filesystem;

#include "lib.cpp"

//тип для описания состояния режима повтора
enum repeat_mode_t { play_once = 0, repeat_one = 1, repeat_all = 2 };

// для ввода из файла в stringstream целой строки
std::fstream &operator>>(std::fstream &fstr, std::stringstream &ss);

// для ввода пары строк
bool operator>>(std::fstream &fstr,
                std::pair<std::stringstream, std::stringstream> &namefile_pair);

// открыть аудиофайл @filenamepath и воспроизвести через @music
void openPlay(sf::Music &music,
              std::pair<std::string, std::string> const &filenamepath,
              nana::slider &seeker, nana::label &lab, nana::button &play_pause,
              bool &play_next);

// форматирование времени
std::string formatTime(int sec);