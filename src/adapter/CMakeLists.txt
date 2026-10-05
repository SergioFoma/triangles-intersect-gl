set(ADAPTER_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/adapter.cpp
)

target_sources(${ADAPTER} PRIVATE ${ADAPTER_SRC})
target_include_directories(${ADAPTER} PUBLIC ${INCLUDE}/${ADAPTER})
