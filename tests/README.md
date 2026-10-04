# Тесты

- `point_test.cpp`: координаты, валидность и печать точки.
- `triangle_test.cpp`: создание треугольника, исключения и компактная таблица
  копланарных случаев.
- `noncoplanar_triangle_test.cpp`: 6 общих тестов и 18 граничных случаев.
  Каждый проверяет все 36 сочетаний перестановок вершин и оба порядка аргументов.
  Касание считается пересечением.

## Граничные случаи

Группа `NonCoplanarCornerCases`. Опорный треугольник:
`A = {(0,0,0), (4,0,0), (0,4,0)}`. Его сечение плоскостью `x = 1` —
отрезок `0 <= y <= 3`, `z = 0`. Координаты второго треугольника явно заданы
в таблице `kCornerCases`.

| Сценарий | Ожидается |
|---|---|
| `PartialSliceOverlap`, `ContainsReferenceSlice` — сечения перекрываются | `true` |
| `DisjointSlicesBefore`, `DisjointSlicesAfter` — обе плоскости пересекают треугольники, но сечения раздельны | `false` |
| `SharedEdge`, `PartialSharedEdge`, `ContainedEdge` — общее ребро или его часть | `true` |
| `DisjointCollinearEdges` — рёбра на одной прямой, но раздельны | `false` |
| `EdgeOnInterior`, `VertexOnInterior`, `VertexOnEdge`, `SharedVertex` — касание | `true` |
| `VertexOnPlaneOutside` — вершина на плоскости, но вне треугольника | `false` |
| `VertexOnPlaneOthersStraddle` — одна вершина на плоскости, две по разные стороны | `true` |
| `JustInsideEdge` — сечение начинается при `y = 3 - 1e-6` | `true` |
| `EdgesTouchAtOnePoint` — сечение начинается ровно при `y = 3` | `true` |
| `JustOutsideEdge` — сечение начинается при `y = 3 + 1e-6` | `false` |
| `NearlyParallelPlanes` — плоскости почти параллельны, но пересечение есть | `true` |

Смещения `1e-6` заметно больше `kEps`: тесты различают геометрическое касание
и промах без предположений о поведении на границе численного допуска.

## Запуск

```sh
cmake -S . -B build
cmake --build build --target triangles_tests
ctest --test-dir build --output-on-failure
```

Только некомпланарные граничные случаи:

```sh
./build/tests/triangles_tests --gtest_filter='*NonCoplanarCornerCases*'
```

Все некомпланарные тесты: `--gtest_filter='*NonCoplanar*'`.
