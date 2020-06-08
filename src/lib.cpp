// #include "SoundFileReaderMp3.hpp"
#include "lib.hpp"

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
  seeker.maximum(static_cast<unsigned int>(music.getDuration().asMilliseconds()));
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

void timer_update_event(nana::slider &seeker, sf::Music &music,
                        nana::button &play_pause, nana::label &lab,
                        repeat_mode_t &state,
                        std::vector<std::pair<std::string, std::string>> &files,
                        nana::label &timestamp, int &current_playing,
                        bool &play_next) {
  // проверяем статус объекта плеера (если музыка играет ...)
  if (music.getStatus() == sf::Music::Playing) {
    // текущая позиция по аудиофайлу относительно начала
    auto offset = music.getPlayingOffset();
    // устанавливаем трекер на значение позиции в миллисекундах
    seeker.value(offset.asMilliseconds());
    // общее количество секунд позиции целым числом
    int sec = static_cast<int>(offset.asSeconds());
    // устанавливаем строку в объект для отображения, предварительно
    // отформатировав
    timestamp.caption(formatTime(sec));
  }
  // проверяем (осталось ли до конца аудиофайла меньше 150 миллисекунд и
  // воспроизводится ли что-нибудь вообще) или сработал флаг из строки 131
  if ((music.getDuration().asMilliseconds() -
               music.getPlayingOffset().asMilliseconds() <
           150 &&
       current_playing != -1) ||
      play_next) {
    // если режим воспроизведения - воспроизводить все
    if (state == repeat_mode_t::repeat_all) {
      // если номер след аудииофайла находтся в пределах индексов списка
      if (static_cast<unsigned int>(++current_playing) < files.size()) {
        // открываем и воспроизводим аудиофайл по индексу
        openPlay(music, files[static_cast<unsigned int>(current_playing)], seeker, lab, play_pause,
                 play_next);
      } else
        // иначе устанавливаем индекс на -1, чтобы остановить повтор
        current_playing = -1;
    }
  } else if (current_playing == -1) {
    // устанавливаем надпись на кнопке
    play_pause.caption("Stop");
    // останавливааем воспроизведение
    music.stop();
  }
}

void open_playlist(nana::filebox &filedialog, int &current_playing,
                   std::vector<std::pair<std::string, std::string>> &files,
                   nana::listbox &playlistbox, bool &play_next) {
  // показываем окно открытия файла и записываем путь до файла/ов как
  // результат
  auto paths = filedialog.show();
  // устанавливаем индекс воспроизведения на -1, грубо говоря обнуляем
  current_playing = -1;
  // очищаем предыдущий массив
  files.clear();
  // очищаем список пред воспроизведения
  playlistbox.clear(1);
  // устанавливаем имя по умолчанию
  playlistbox.at(1).text("<unnamed playlist>");

  // строка имени плейлиста
  std::stringstream name{};
  name.str("");

  // проходимся по всем выбранным файлам
  for (auto file : paths) {
    // если расширени файла .plst
    if (file.extension().string() == ".plst") {
      // открываем файл для чтения
      std::fstream filestream{file, std::ios_base::in};
      // если не открылся - пропускаем
      if (!filestream.is_open())
        continue;
      // добавляем в имя плейлиста имя текущего файла без расширения и пробел
      // в конце
      name << file.filename().replace_extension() << " ";
      // цикл до конца файла
      do {
        // считываем пару строк (в первой имя, во сторой путь)
        std::pair<std::stringstream, std::stringstream> fnpair{};
        if (!(filestream >> fnpair))
          break;
        // добавляем в массив
        files.push_back({fnpair.first.str(), fnpair.second.str()});
      } while (!filestream.eof());
      // закрываем файл
      filestream.close();
    } else
      // иначе просто добавляем файл в массив
      files.push_back(
          {file.filename().replace_extension().string(), file.string()});
  }
  // выбираем категорию 1 (плейлист)
  auto cat = playlistbox.at(1);
  // добавляем в категорию 1 списка имена и пути до аудиофайлов
  for (auto i : files)
    cat.append({i.first, i.second});
  // обнуляем флаг
  play_next = 0;
}

void save_playlist(nana::filebox &listsavedialog,
                   std::vector<std::pair<std::string, std::string>> &files,
                   nana::listbox &playlistbox) {
  // показываем окно для выбора имени файла
  auto file = listsavedialog.show();
  // если не выбрали выходим из функции обработчика
  if (file.size() == 0)
    return;
  // открываем выбранный файл на запись
  std::fstream filestream{file[0], std::ios::out};
  // проходимся по массиву аудиофайлов
  for (auto i : files)
    // для каждого записываем имя и путь в разных строках
    filestream << i.first << "\n" << i.second << "\n";
  // закрываем файл
  filestream.close();
  // устанавливаем надпись имени плейлиста на выбранное имя файла без
  // расшиирения
  playlistbox.at(1).text(file[0].filename().replace_extension());
}