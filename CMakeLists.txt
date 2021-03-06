cmake_minimum_required (VERSION 3.0)
project (digiham VERSION 0.4.0)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

find_package(LibMbe REQUIRED)

INCLUDE_DIRECTORIES(
    "${LIBMBE_INCLUDE_DIR}"
)

SET(LIBS ${LIBS} ${LIBMBE_LIBRARY})

add_library(ysf_trellis STATIC ysf/trellis.c)
target_include_directories (ysf_trellis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(ysf_fich STATIC ysf/fich.c)
target_include_directories (ysf_fich PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(ysf_whitening STATIC ysf/whitening.c)
target_include_directories (ysf_whitening PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(version STATIC version.c)
target_include_directories (version PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(dmr_quadratic_residue STATIC dmr/quadratic_residue.c)
target_include_directories (dmr_quadratic_residue PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(dmr_hamming_16_11 STATIC dmr/hamming_16_11.c)
target_include_directories (dmr_hamming_16_11 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(dmr_hamming_7_4 STATIC dmr/hamming_7_4.c)
target_include_directories (dmr_hamming_7_4 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(ysf_golay_24_12 STATIC ysf/golay_24_12.c)
target_include_directories (ysf_golay_24_12 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(ysf_crc16 STATIC ysf/crc16.c)
target_include_directories (ysf_crc16 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(dmr_golay_20_8 STATIC dmr/golay_20_8.c)
target_include_directories (dmr_golay_20_8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(dmr_hamming_15_11 STATIC dmr/hamming_15_11.c)
target_include_directories (dmr_hamming_15_11 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(dmr_hamming_13_9 STATIC dmr/hamming_13_9.c)
target_include_directories (dmr_hamming_13_9 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(dmr_bptc_196_96 STATIC dmr/bptc_196_96.c)
target_include_directories (dmr_bptc_196_96 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(dmr_bptc_196_96 dmr_hamming_13_9 dmr_hamming_15_11)
add_library(ysf_gps STATIC ysf/gps.c)
target_include_directories (ysf_gps PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(ysf_radio_types STATIC ysf/radio_types.c)
target_include_directories (ysf_radio_types PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(hamming_distance STATIC hamming_distance.c)
target_include_directories (hamming_distance PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(dumphex STATIC dumphex.c)
target_include_directories (dumphex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(pocsag_bch_31_21 STATIC pocsag/bch_31_21.c)
target_include_directories (pocsag_bch_31_21 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(rrc_filter rrc_filter.c)
target_link_libraries(rrc_filter version)
add_executable(dc_block dc_block.c)
target_link_libraries(dc_block version)
add_executable(gfsk_demodulator gfsk_demodulator.c)
target_link_libraries(gfsk_demodulator version)
add_executable(dmr_decoder dmr_decoder.c)
target_link_libraries(dmr_decoder version dmr_quadratic_residue dmr_hamming_16_11 dmr_hamming_7_4 dmr_golay_20_8 dmr_hamming_15_11 dmr_hamming_13_9 dmr_bptc_196_96 hamming_distance dumphex)
add_executable(ysf_decoder ysf_decoder.c)
target_link_libraries(ysf_decoder ysf_trellis ysf_golay_24_12 ysf_fich ysf_whitening version ysf_crc16 ysf_gps ysf_radio_types hamming_distance)
add_executable(mbe_synthesizer mbe_synthesizer.c)
target_link_libraries(mbe_synthesizer version ${LIBS})
add_executable(digitalvoice_filter digitalvoice_filter.c)
target_link_libraries(digitalvoice_filter version)
add_executable(fsk_demodulator fsk_demodulator.c)
target_link_libraries(fsk_demodulator version)
add_executable(pocsag_decoder pocsag_decoder.c)
target_link_libraries(pocsag_decoder version hamming_distance dumphex pocsag_bch_31_21)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(dmr_quadratic_residue_syndrome_generator dmr/quadratic_residue_syndrome_generator.c)
    target_link_libraries(dmr_quadratic_residue_syndrome_generator dmr_quadratic_residue ${LIBS})
    add_executable(dmr_hamming_16_11_syndrome_generator dmr/hamming_16_11_syndrome_generator.c)
    target_link_libraries(dmr_hamming_16_11_syndrome_generator dmr_hamming_16_11 ${LIBS})
    add_executable(dmr_hamming_7_4_syndrome_generator dmr/hamming_7_4_syndrome_generator.c)
    target_link_libraries(dmr_hamming_7_4_syndrome_generator dmr_hamming_7_4 ${LIBS})
    add_executable(ysf_golay_24_12_syndrome_generator ysf/golay_24_12_syndrome_generator.c)
    target_link_libraries(ysf_golay_24_12_syndrome_generator ysf_golay_24_12 ${LIBS})
    add_executable(dmr_golay_20_8_syndrome_generator dmr/golay_20_8_syndrome_generator.c)
    target_link_libraries(dmr_golay_20_8_syndrome_generator dmr_golay_20_8 ${LIBS})
    add_executable(dmr_hamming_15_11_syndrome_generator dmr/hamming_15_11_syndrome_generator.c)
    target_link_libraries(dmr_hamming_15_11_syndrome_generator dmr_hamming_15_11 ${LIBS})
    add_executable(dmr_hamming_13_9_syndrome_generator dmr/hamming_13_9_syndrome_generator.c)
    target_link_libraries(dmr_hamming_13_9_syndrome_generator dmr_hamming_13_9 ${LIBS})
    add_executable(pocsag_bch_31_21_syndrome_generator pocsag/bch_31_21_syndrome_generator.c)
    target_link_libraries(pocsag_bch_31_21_syndrome_generator pocsag_bch_31_21 ${LIBS})
endif()

install(TARGETS rrc_filter dc_block gfsk_demodulator dmr_decoder ysf_decoder mbe_synthesizer digitalvoice_filter fsk_demodulator pocsag_decoder DESTINATION bin)

SET(CMAKE_CXX_FLAGS "-O3")
SET(CMAKE_C_FLAGS "-O3")

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jakob Ketterl <jakob.ketterl@gmx.de>")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Tools for decoding digital ham radio communication")
SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS "ON")
SET(CPACK_DEBIAN_PACKAGE_SECTION "hamradio")
if(DEFINED ENV{BUILD_NUMBER})
  SET(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}-$ENV{BUILD_NUMBER}")
endif()
INCLUDE(CPack)
