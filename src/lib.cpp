// #include "SoundFileReaderMp3.hpp"
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

// для ввода из файла в stringstream целой строки
std::fstream &operator>>(std::fstream &fstr, std::stringstream &ss) {
  char buf;
  do {
    fstr.get(buf);
    if (buf != '\n' && buf != '\0')
      ss << buf;
  } while (buf != '\n' && buf != '\0');
  return fstr;
}

// для ввода пары строк
bool operator>>(
    std::fstream &fstr,
    std::pair<std::stringstream, std::stringstream> &namefile_pair) {
  namefile_pair.first.str("");
  namefile_pair.second.str("");
  fstr >> namefile_pair.first >> namefile_pair.second;
  if (namefile_pair.second.str() == "")
    return 0;
  return 1;
}

// открыть аудиофайл @filenamepath и воспроизвести через @music
void openPlay(sf::Music &music,
              std::pair<std::string, std::string> const &filenamepath,
              nana::slider &seeker, nana::label &lab, nana::button &play_pause,
              bool &play_next) {
  // открываем аудиофайл и записываем результат
  // (открылся или нет)
  bool opened = music.openFromFile(filenamepath.second);
  // запускаем воспроизведение в отдельномм потоке
  music.play();
  // устанавливаемм максимальное значание полосы прокрутки
  seeker.maximum(music.getDuration().asMilliseconds());
  // проверяем открылся ли файл
  if (opened) {
    // устанавливаем название воспроизводимого файла
    lab.caption("Playing: <bold>" + filenamepath.first + "</>");
    // устанавливаем значание полосы прокрутки
    seeker.value(music.getPlayingOffset().asMilliseconds());
    // надпись на кнопке play/pause/stop
    play_pause.caption("Play");
    // сбрасываем флаг
    play_next = 0;
  } else {
    // иначе отображаем ошибку
    lab.caption("<bold>Error! File not found or format is incorrect.</>");
    // надпись на кнопке
    play_pause.caption("Stop");
    // устанавливаем флаг чтобы пропустить невалидный файл
    play_next = 1;
  }
}

// форматирование времени
std::string formatTime(int sec) {
  // разбиваем на часы, минуты, секунды
  int hours = sec / 3600, minutes = (sec / 60) % 60, seconds = sec % 60;
  // для записи всего этого в одну строку с разделителем
  std::stringstream ss;
  ss.str("");
  // "std::setfill('0') << std::setw(2)" для добавления лидирующих нулей
  ss << std::setfill('0') << std::setw(2) << hours << ':' << std::setfill('0')
     << std::setw(2) << minutes << ':' << std::setfill('0') << std::setw(2)
     << seconds;
  return ss.str();
}