set(HTTP_SRC
        http/HttpParser.cc
        http/HttpReponse.cc
        http/HttpServer.cc
        http/main.cc
)

add_executable(httpserver ${HTTP_SRC})
target_link_libraries(httpserver tinymuduo)

# for test
add_executable(httpparser_test http/test/httpparser_test.cc http/HttpParser.cc)
target_link_libraries(httpparser_test tinymuduo gtest)




