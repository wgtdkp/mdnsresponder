add_library(srp-client
    sign-mbedtls.c
    srp-client.c
    towire.c
)

target_compile_definitions(srp-client
    PUBLIC SRP_CRYPTO_MBEDTLS=1
)

target_include_directories(srp-client
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(srp-client
    PRIVATE
        ${OT_MBEDTLS}
        mbedcrypto
        ot-config
)
