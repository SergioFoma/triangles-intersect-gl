# Bизуализация на OpenGL кубического объёма с пересекающимися треугольниками 🔺

This is C++ Vladimirov task.
Collaborator pr1usf0x.

![C++](https://img.shields.io/badge/C++-23-blue?logo=cplusplus)
![CMake](https://img.shields.io/badge/CMake-3.14-064F8C?logo=cmake)
![GoogleTest](https://img.shields.io/badge/GoogleTest-passing-4285F4?logo=googletest&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-4.1-5586A4?logo=opengl&logoColor=white)

## Библиотеки, необходимые для работы программы 📚

![OpenGL](https://img.shields.io/badge/OpenGL-4.1-5586A4?logo=opengl&logoColor=white)
![GLFW](https://img.shields.io/badge/GLFW-3.3-green?logo=glfw&logoColor=white)
![TBB](https://img.shields.io/badge/TBB-oneAPI-0071C5?logo=logoColor=white)

## Структура проекта 📝

```
├──📂triangles-intersect-gl
│   ├──📂blue                     # png изображения с текстурами для skybox
│   ├──📂include                  # заголовочные файлы проекта
│   │   ├──📂graphics             # заголовочные файла для графики
│   │   └──📂triangles            # заголовочные файлы для анализатора пересечений
|   │   ├──📄adapter.hpp          # паттерн проектирования
|   │   ├──📄read_data.hpp        # чтение входных данных из файла
│   ├──📂src                      # исходники
│   │   ├── graphics              # исходники для графики
│   │   └── triangles             # исходники для анализатора пересечений
|   │   ├──📄adapter.cpp          # паттерн проектирования
|   │   ├──📄read_data.cpp        # чтение входных данных из файла
|   │   ├──📄main.cpp             # программа пользователя
|   │   ├──📄CMakeLists.txt       # cmake для сборки исходников
│   ├── 📂materials               # файлы для unit и end-to-end тестирования
│   ├── 📂shaders                 # шейдеры
│   └── 📂tests                   # исходники, реализующие тестирование
│   ├──📄CMakeLists.txt           # cmake для сборки проекта
```

## Скачивание репозитория 👾

### **Чтобы скачать весь проект, нужно:**

1. В Вашем терминале необходимо перейти в директорию, куда Вы хотите скачать данный проект.

2. Затем в терминале введите:

SSH:

```bash
git clone git@github.com:SergioFoma/triangles-intersect-gl.git
```

HTTPS:

```bash
git clone https://github.com/SergioFoma/triangles-intersect-gl.git
```

3. Дождитесь скачивания.

4. После чего будет создана рабочая директория с названием репозитория (triangles-intersect-gl).

## Запуск основой программы 🚀

Для сборки проект необходимо перейти в его корень и выполнить команду:

```bash
cmake -S . -B Build
```

Для компиляции проекта выполните команду:

```bash
cmake --build Build
```

**Опции программы**:

Для просмотра возможных команд выполните:

```bash
Build/bin/triangle_intersect --help

Finding triangles intersection and rendering them


Build/bin/triangle_intersect [OPTIONS] input_name


POSITIONALS:
  input_name TEXT REQUIRED    The input data file

OPTIONS:
  -h,     --help              Print this help message and exit
  -f,     --input_file TEXT REQUIRED
                              The input data file
          --only-intersection-analysis
                              Analyzes only intersection
```

Запуск поиска пересечений для случая миллиона треугольников и их отрисовка:

```bash
Build/bin/triangle_intersect -f materials/triangles_1000000.txt
```

## Запуск тестов ⚙

Соберите проект с флагом ```-DBUILD_TESTING=true```:

```bash
cmake -S . -B Build -DBUILD_TESTING=true
```

Компиляции unit и end-to-end тестирования:

```bash
cmake --build Build
```

Запуск ```google тестов```:

```bash
Build/bin/triangles_tests
```

Запуск ```end-to-end тестов```:

```bash
Build/bin/end_to_end_testing
```

Запуск ```google benchmark```:

```bash
Build/bin/benchmark_test
```

## Collaborators 👤

[pr1usf0x](https://github.com/pr1usf0x)
