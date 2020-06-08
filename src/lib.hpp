#pragma once

#include <chrono>
#include <experimental/filesystem>

using namespace std::literals::chrono_literals;
namespace fs = std::experimental::filesystem;

#include <SFML/Audio.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nana/gui.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <string>

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

// функция обработчик событий таймера
void timer_update_event(nana::slider &seeker, sf::Music &music,
                        nana::button &play_pause, nana::label &lab,
                        repeat_mode_t &state,
                        std::vector<std::pair<std::string, std::string>> &files,
                        nana::label &timestamp, int &current_playing,
                        bool &play_next);

// функция обработчик события нажатия кнопки "открыть"
void open_playlist(nana::filebox &filedialog, int &current_playing,
                   std::vector<std::pair<std::string, std::string>> &files,
                   nana::listbox &playlistbox, bool &play_next);

// функция обработчик события нажатия кнопки "сохраниить плейлист"
void save_playlist(nana::filebox &listsavedialog,
                   std::vector<std::pair<std::string, std::string>> &files,
                   nana::listbox &playlistbox);