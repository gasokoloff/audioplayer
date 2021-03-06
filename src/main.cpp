// #include "SoundFileReaderMp3.hpp"
#include "lib.hpp"

int main() {
  // массив пар названий и путей до аудиофайлов
  std::vector<std::pair<std::string, std::string>> files{};
  // Layout разметка окна
  std::string div =
      "vert <><<text>><>"
      "<<timestamp>><>"
      "<<seeker>><>"
      "<height=70% <><weight=96% playlist><>><>"
      "<<><weight=20% button><><weight=20% play_pause><><weight=20% "
      "repeat><><weight=20% save_list><>><>";

  // mp3 custom support (doesn't work)
  // sf::SoundFileFactory::registerReader<audio::SoundFileReaderMp3>();
  // sfml audio библиотека для воспроизведения аудио
  sf::Music music;
  // включить повторное воспроиизведение defauld mode = repeat_one
  music.setLoop(1);
  int current_playing = -1;
  repeat_mode_t state = repeat_mode_t::repeat_one;

  // объект окна
  nana::form fm;
  // label окна
  fm.caption("Audio Player");
  // размеры окна
  fm.size({600, 700});

  // объект списка воспроизведения
  nana::listbox playlistbox{fm};
  // добавляем заголовки
  playlistbox.append_header("Name", static_cast<unsigned int>(300.0 * 0.94));
  playlistbox.append_header("Path", static_cast<unsigned int>(300.0 * 0.94));
  // добавляем наименование плейлиста в поле для имени группы в объекте списка
  playlistbox.append("<unnamed playlist>");
  // вклбчаем автоматическую перерисовку объекта
  playlistbox.auto_draw(true);
  // разрешаем выбор только одного элемента списка
  playlistbox.enable_single(true, true);

  // отображение названия воспроизводимого аудиофайла
  nana::label lab{fm, "Playing: <bold>Sample</>"};
  // выравниваем текст по центру
  lab.text_align(nana::align::center);
  // включаем форматирование текста (html)
  lab.format(true);

  // кнопка открытия файла
  nana::button btn{fm, "Open file"};
  // кнопка play/pause/stop
  nana::button play_pause{fm, "Stop"};
  // кнопка сохранения списка воспроизведения
  nana::button save_list{fm, "Save playlist"};
  // кнопка для выбора режима повтора
  nana::button repeat{fm, "Repeat one"};

  // окно выбора файлов
  nana::filebox filedialog{fm, true};
  // указываем путь, который будет отображаться в этом окне по умолчанию
  filedialog.init_path("./");
  // указываем фильтры для выбора файлов
  filedialog.add_filter(
      {{"All suppoted audio", "*.ogg;*.wav;*.flac"},
       {"Playlist file", ".plst"},
       {"OGG", ".ogg"},
       {"WAV", ".wav"},
       {"FLAC", ".flac"},
       {"MP3 - doesnt work because of licence issues", ".mp3"}});
  // разрешаем выбор несколькких файлов
  filedialog.allow_multi_select(true);
  // окно сохранения списка воспроизведения
  nana::filebox listsavedialog{fm, false};
  // имя файла по умолчанию
  listsavedialog.init_file("./unnamed_list.plst");
  // фильтр
  listsavedialog.add_filter("Playlist file", ".plst");

  // прокрутка воспроизведения
  nana::slider seeker{fm, true};
  // максимальное значение прокрутки (изначально будет 0)
  seeker.maximum(
      static_cast<unsigned int>(music.getDuration().asMilliseconds()));
  // отображение времени воспроизведения
  nana::label timestamp{fm, true};
  timestamp.text_align(nana::align::center);
  seeker.size({10, 300});
  // объект таймера для обновление полосы прокрутки и отображения времени с
  // частотой обновления 10Гц
  nana::timer update_timer{100ms};
  // флаг для перехода к след адио файлу, если текущий не валиден
  bool play_next = false;

  // далее следуют объявления обработчиков событий
  // обработчик обновления таймера
  update_timer.elapse([&] {
    timer_update_event(seeker, music, play_pause, lab, state, files, timestamp,
                       current_playing, play_next);
  });
  // update_timer.elapse(std::bind(&timer_update_event, seeker, music,
  // play_pause, lab, state, files, timestamp, current_playing, play_next));

  // действие при изменинии значания полосы прокрутки пользователем
  seeker.events().value_changed([&seeker, &music] {
    // узнаем значение полосы прокрутки
    auto val = seeker.value();
    // переводим в объект типа sf::Time считая, что val показывает значение в
    // миллисекундах, и перематываем музыку на это значение от начала
    music.setPlayingOffset(sf::milliseconds(static_cast<int>(val)));
  });

  // действие при двойном клике на элемент списка воспроизведения
  playlistbox.events().dbl_click([&music, &lab, &seeker, &play_pause,
                                  &playlistbox, &files, &current_playing,
                                  &play_next] {
    // получаем массив выбранных элементов в списке (1 или 0, потому что
    // выбирать моожно только один)
    auto sel = playlistbox.selected();
    // проверяем выбрано ли что-то
    if (sel.size() > 0) {
      // получаем индекс этого значения в списке (а соответственно и в массиве
      // files)
      current_playing = static_cast<int>(sel[0].item);
      // получаем пару имя и путь до файла по индексу
      auto i = files[static_cast<unsigned long>(current_playing)];

      // загружаем и запускаем
      openPlay(music, i, seeker, lab, play_pause, play_next);
    }
    // сбрасываем флаг
    play_next = 0;
  });

  // действие при нажатии на кнопку "открыть файл"
  btn.events().click([&] {
    open_playlist(filedialog, current_playing, files, playlistbox, play_next);
  });

  // действие при нажатии на кнопку play/pause/stop
  play_pause.events().click([&play_pause, &music, &files, &lab, &seeker,
                             &current_playing, &play_next] {
    // если музыка играет
    if (music.getStatus() == sf::SoundSource::Playing) {
      // меняем надпись
      play_pause.caption("Pause");
      // ставим на паузу
      music.pause();
    } else {
      // меняем надпись
      play_pause.caption("Play");
      // проверяем остановлена ли музыка (если просто стояла на паузе будет
      // sf::SoundSource::Paused)
      if (music.getStatus() == sf::SoundSource::Stopped) {
        // проверяем есть ли загруженные файлы
        if (files.size() > 0) {
          // включаем первый из массива
          current_playing = 0;
          openPlay(music, files[static_cast<unsigned long>(current_playing)],
                   seeker, lab, play_pause, play_next);
        } else {
          // устанавливаем надпись о том, что треков для воспроизведения не
          // обнаружено
          lab.caption("No files to play");
          return;
        }
      }
      // запускаем
      music.play();
    }
  });

  // действия при клике на кнопку сохранить
  save_list.events().click([&] {
    save_playlist(listsavedialog, files, playlistbox);
  });

  // действия при клике на кнопку выбора режима повтора
  repeat.events().click([&repeat, &state, &music] {
    // устанавливаем следующее значение режима по порядку
    state = static_cast<repeat_mode_t>((state + 1) % 3);
    // в зависимости от выбранного режима устанавливаем текст и повтор
    // аудиофайла при достижении конца файла
    switch (state) {
    case (repeat_mode_t::play_once):
      repeat.caption("Once");
      music.setLoop(0);
      break;
    case (repeat_mode_t::repeat_all):
      repeat.caption("Repeat all");
      music.setLoop(1);
      break;
    case (repeat_mode_t::repeat_one):
      repeat.caption("Repeat one");
      music.setLoop(1);
      break;
    }
  });

  // Layout management
  fm.div(div);
  fm["text"] << lab;
  fm["button"] << btn;
  fm["seeker"] << seeker;
  fm["playlist"] << playlistbox;
  fm["timestamp"] << timestamp;
  fm["play_pause"] << play_pause;
  fm["save_list"] << save_list;
  fm["repeat"] << repeat;
  fm.collocate();

  // запускаем таймер
  update_timer.start();

  // Show the form
  fm.show();

  // Start to event loop process, it blocks until the form is closed.
  nana::exec();

  return 0;
}