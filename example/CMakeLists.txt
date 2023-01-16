set(HTTP_SRC
        http/HttpParser.cc
        http/HttpReponse.cc
        http/HttpServer.cc
        http/main.cc
)

add_executable(httpserver ${HTTP_SRC})
target_link_libraries(httpserver tinymuduo)

# for test



