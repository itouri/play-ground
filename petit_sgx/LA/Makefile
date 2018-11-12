#
# Copyright (C) 2011-2018 Intel Corporation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#   * Neither the name of Intel Corporation nor the names of its
#     contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#

######## SGX SDK Settings ########

SGX_SDK ?= /opt/intel/sgxsdk
SGX_MODE ?= HW
SGX_ARCH ?= x64
SGX_DEBUG ?= 1

ifeq ($(shell getconf LONG_BIT), 32)
	SGX_ARCH := x86
else ifeq ($(findstring -m32, $(CXXFLAGS)), -m32)
	SGX_ARCH := x86
endif

ifeq ($(SGX_ARCH), x86)
	SGX_COMMON_FLAGS := -m32
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x86/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x86/sgx_edger8r
else
	SGX_COMMON_FLAGS := -m64
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib64
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x64/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x64/sgx_edger8r
endif

ifeq ($(SGX_DEBUG), 1)
ifeq ($(SGX_PRERELEASE), 1)
$(error Cannot set SGX_DEBUG and SGX_PRERELEASE at the same time!!)
endif
endif

ifeq ($(SGX_DEBUG), 1)
	SGX_COMMON_FLAGS += -O0 -g
else
	SGX_COMMON_FLAGS += -O2
endif

SGX_COMMON_FLAGS += -v -Wall -Wextra -Winit-self -Wpointer-arith -Wreturn-type \
                    -Waddress -Wsequence-point -Wformat-security \
                    -Wmissing-include-dirs -Wfloat-equal -Wundef -Wshadow \
                    -Wcast-align -Wcast-qual -Wconversion -Wredundant-decls
SGX_COMMON_CFLAGS := $(SGX_COMMON_FLAGS) -Wjump-misses-init -Wstrict-prototypes -Wunsuffixed-float-constants
SGX_COMMON_CXXFLAGS := $(SGX_COMMON_FLAGS) -Wnon-virtual-dtor -std=c++11

######## Library Settings ########

Trust_Lib_Name := libLocalAttestation_Trusted.a
TrustLib_Cpp_Files := $(wildcard LocalAttestationCode/*.cpp)
TrustLib_Cpp_Objects := $(TrustLib_Cpp_Files:.cpp=.o)
TrustLib_Include_Paths := -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx -I./Include #-I$(SGX_SDK)/include/epid -I./Include
TrustLib_C_Flags := -nostdinc -fvisibility=hidden -fpie -fstack-protector  $(TrustLib_Include_Paths)
TrustLib_Cxx_Flags :=  $(TrustLib_C_Flags) $(SGX_COMMON_CXXFLAGS) -nostdinc++
TrustLib_C_Flags += $(SGX_COMMON_CFLAGS) 

UnTrustLib_Name := libLocalAttestation_unTrusted.a
UnTrustLib_Cpp_Files := $(wildcard Untrusted_LocalAttestation/*.cpp)
UnTrustLib_Cpp_Objects := $(UnTrustLib_Cpp_Files:.cpp=.o)
UnTrustLib_Include_Paths := -I$(SGX_SDK)/include -I./Include -I./LocalAttestationCode
UnTrustLib_Cxx_Flags := $(SGX_COMMON_CXXFLAGS) -fPIC -Wno-attributes $(UnTrustLib_Include_Paths)

######## App Settings ########

ifneq ($(SGX_MODE), HW)
	Urts_Library_Name := sgx_urts_sim
else
	Urts_Library_Name := sgx_urts
endif

App_Cpp_Files := $(wildcard App/*.cpp)
App_Include_Paths := -I$(SGX_SDK)/include -I./Include -I./LocalAttestationCode

App_C_Flags := -fPIC -Wno-attributes $(App_Include_Paths)
# Three configuration modes - Debug, prerelease, release
#   Debug - Macro DEBUG enabled.
#   Prerelease - Macro NDEBUG and EDEBUG enabled.
#   Release - Macro NDEBUG enabled.
ifeq ($(SGX_DEBUG), 1)
	App_C_Flags += -DDEBUG -UNDEBUG -UEDEBUG
else ifeq ($(SGX_PRERELEASE), 1)
	App_C_Flags += -DNDEBUG -DEDEBUG -UDEBUG
else
	App_C_Flags += -DNDEBUG -UEDEBUG -UDEBUG
endif

App_Cxx_Flags := $(App_C_Flags) $(SGX_COMMON_CXXFLAGS)
App_C_Flags += $(SGX_COMMON_CFLAGS)

App_Link_Flags := -L$(SGX_LIBRARY_PATH) -l$(Urts_Library_Name) -L. -lpthread -lLocalAttestation_unTrusted 

ifneq ($(SGX_MODE), HW)
	App_Link_Flags += -lsgx_uae_service_sim
else
	App_Link_Flags += -lsgx_uae_service
endif

App_Cpp_Objects := $(App_Cpp_Files:.cpp=.o)
App_Name := master_enclave_app

######## Enclave Settings ########

MasterEnclave_Version_Script := MasterEnclave/MasterEnclave.lds
GrapheneEnclave_Version_Script := GrapheneEnclave/GrapheneEnclave.lds

ifneq ($(SGX_MODE), HW)
	Trts_Library_Name := sgx_trts_sim
	Service_Library_Name := sgx_tservice_sim
else
	Trts_Library_Name := sgx_trts
	Service_Library_Name := sgx_tservice
endif
Crypto_Library_Name := sgx_tcrypto

Enclave_Cpp_Files_1 := $(wildcard MasterEnclave/*.cpp)
Enclave_Cpp_Files_2 := $(wildcard GrapheneEnclave/*.cpp)
Enclave_Include_Paths := -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx -I./LocalAttestationCode -I./Include

CC_BELOW_4_9 := $(shell expr "`$(CC) -dumpversion`" \< "4.9")
ifeq ($(CC_BELOW_4_9), 1)
	Enclave_C_Flags := -nostdinc -fvisibility=hidden -fpie -ffunction-sections -fdata-sections -fstack-protector
else
	Enclave_C_Flags := -nostdinc -fvisibility=hidden -fpie -ffunction-sections -fdata-sections -fstack-protector-strong
endif

Enclave_C_Flags += $(Enclave_Include_Paths)
Enclave_Cxx_Flags := $(Enclave_C_Flags) $(SGX_COMMON_CXXFLAGS) -nostdinc++
Enclave_C_Flasg += $(SGX_COMMON_CFLAGS)

# To generate a proper enclave, it is recommended to follow below guideline to link the trusted libraries:
#    1. Link sgx_trts with the `--whole-archive' and `--no-whole-archive' options,
#       so that the whole content of trts is included in the enclave.
#    2. For other libraries, you just need to pull the required symbols.
#       Use `--start-group' and `--end-group' to link these libraries.
# Do NOT move the libraries linked with `--start-group' and `--end-group' within `--whole-archive' and `--no-whole-archive' options.
# Otherwise, you may get some undesirable errors.
Common_Enclave_Link_Flags := -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tcxx -l$(Crypto_Library_Name) -L. -lLocalAttestation_Trusted -l$(Service_Library_Name) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 -Wl,--gc-sections
MasterEnclave_Link_Flags := $(Common_Enclave_Link_Flags) -Wl,--version-script=$(MasterEnclave_Version_Script)
GrapheneEnclave_Link_Flags := $(Common_Enclave_Link_Flags) -Wl,--version-script=$(GrapheneEnclave_Version_Script)

Enclave_Cpp_Objects_1 := $(Enclave_Cpp_Files_1:.cpp=.o)
Enclave_Cpp_Objects_2 := $(Enclave_Cpp_Files_2:.cpp=.o)

Enclave_Name_1 := libMasterenclave.so
Enclave_Name_2 := libGrapheneenclave.so

ifeq ($(SGX_MODE), HW)
ifeq ($(SGX_DEBUG), 1)
	Build_Mode = HW_DEBUG
else ifeq ($(SGX_PRERELEASE), 1)
	Build_Mode = HW_PRERELEASE
else
	Build_Mode = HW_RELEASE
endif
else
ifeq ($(SGX_DEBUG), 1)
	Build_Mode = SIM_DEBUG
else ifeq ($(SGX_PRERELEASE), 1)
	Build_Mode = SIM_PRERELEASE
else
	Build_Mode = SIM_RELEASE
endif
endif

ifeq ($(Build_Mode), HW_RELEASE)
all: .config_$(Build_Mode)_$(SGX_ARCH) $(Trust_Lib_Name) $(UnTrustLib_Name) MasterEnclave.so GrapheneEnclave.so Enclave3.so $(App_Name)
	@echo "The project has been built in release hardware mode."
	@echo "Please sign the enclaves (MasterEnclave.so, GrapheneEnclave.so, Enclave3.so) first with your signing keys before you run the $(App_Name) to launch and access the enclave."
	@echo "To sign the enclaves use the following commands:"
	@echo "   $(SGX_ENCLAVE_SIGNER) sign -key <key1> -enclave MasterEnclave.so -out <$(Enclave_Name_1)> -config MasterEnclave/MasterEnclave.config.xml"
	@echo "   $(SGX_ENCLAVE_SIGNER) sign -key <key2> -enclave GrapheneEnclave.so -out <$(Enclave_Name_2)> -config GrapheneEnclave/GrapheneEnclave.config.xml"
	@echo "   $(SGX_ENCLAVE_SIGNER) sign -key <key3> -enclave Enclave3.so -out <$(Enclave_Name_3)> -config Enclave3/Enclave3.config.xml"
	@echo "You can also sign the enclaves using an external signing tool."
	@echo "To build the project in simulation mode set SGX_MODE=SIM. To build the project in prerelease mode set SGX_PRERELEASE=1 and SGX_MODE=HW."
else
all: .config_$(Build_Mode)_$(SGX_ARCH) $(Trust_Lib_Name) $(UnTrustLib_Name) $(Enclave_Name_1) $(Enclave_Name_2) $(Enclave_Name_3) $(App_Name)
ifeq ($(Build_Mode), HW_DEBUG)
	@echo "The project has been built in debug hardware mode."
else ifeq ($(Build_Mode), SIM_DEBUG)
	@echo "The project has been built in debug simulation mode."
else ifeq ($(Build_Mode), HW_PRERELEASE)
	@echo "The project has been built in pre-release hardware mode."
else ifeq ($(Build_Mode), SIM_PRERELEASE)
	@echo "The project has been built in pre-release simulation mode."
else
	@echo "The project has been built in release simulation mode."
endif
endif

.config_$(Build_Mode)_$(SGX_ARCH):
	@rm -rf .config_* $(App_Name) *.so *.a App/*.o MasterEnclave/*.o MasterEnclave/*_t.* MasterEnclave/*_u.* GrapheneEnclave/*.o GrapheneEnclave/*_t.* GrapheneEnclave/*_u.* LocalAttestationCode/*.o Untrusted_LocalAttestation/*.o LocalAttestationCode/*_t.* 
	@touch .config_$(Build_Mode)_$(SGX_ARCH)

######## Library Objects ########

LocalAttestationCode/LocalAttestationCode_t.c LocalAttestationCode/LocalAttestationCode_t.h : $(SGX_EDGER8R) LocalAttestationCode/LocalAttestationCode.edl
	@cd LocalAttestationCode && $(SGX_EDGER8R) --trusted ../LocalAttestationCode/LocalAttestationCode.edl --search-path $(SGX_SDK)/include 
	@echo "GEN  =>  $@"

LocalAttestationCode/LocalAttestationCode_t.o: LocalAttestationCode/LocalAttestationCode_t.c
	@$(CC) $(TrustLib_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

LocalAttestationCode/%.o: LocalAttestationCode/%.cpp LocalAttestationCode/LocalAttestationCode_t.h
	@$(CXX) $(TrustLib_Cxx_Flags) -c $< -o $@
	@echo "CC   <= $<"

$(Trust_Lib_Name): LocalAttestationCode/LocalAttestationCode_t.o $(TrustLib_Cpp_Objects)
	@$(AR) rcs $@ $^
	@echo "GEN  =>  $@"

Untrusted_LocalAttestation/%.o: Untrusted_LocalAttestation/%.cpp
	@$(CXX) $(UnTrustLib_Cxx_Flags) -c $< -o $@
	@echo "CC   <=  $<"

$(UnTrustLib_Name): $(UnTrustLib_Cpp_Objects)
	@$(AR) rcs $@ $^
	@echo "GEN  =>  $@"

######## App Objects ########
MasterEnclave/MasterEnclave_u.c MasterEnclave/MasterEnclave_u.h: $(SGX_EDGER8R) MasterEnclave/MasterEnclave.edl
	@cd MasterEnclave && $(SGX_EDGER8R) --use-prefix --untrusted ../MasterEnclave/MasterEnclave.edl --search-path $(SGX_SDK)/include 
	@echo "GEN  =>  $@"

App/MasterEnclave_u.o: MasterEnclave/MasterEnclave_u.c
	@$(CC) $(App_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

GrapheneEnclave/GrapheneEnclave_u.c GrapheneEnclave/GrapheneEnclave_u.h: $(SGX_EDGER8R) GrapheneEnclave/GrapheneEnclave.edl
	@cd GrapheneEnclave && $(SGX_EDGER8R) --use-prefix --untrusted ../GrapheneEnclave/GrapheneEnclave.edl --search-path $(SGX_SDK)/include 
	@echo "GEN  =>  $@"

App/GrapheneEnclave_u.o: GrapheneEnclave/GrapheneEnclave_u.c
	@$(CC) $(App_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

App/%.o: App/%.cpp MasterEnclave/MasterEnclave_u.h GrapheneEnclave/GrapheneEnclave_u.h
	@$(CXX) $(App_Cxx_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

$(App_Name): App/MasterEnclave_u.o App/GrapheneEnclave_u.o $(App_Cpp_Objects) $(UnTrustLib_Name)
	@$(CXX) $^ -o $@ $(App_Link_Flags)
	@echo "LINK =>  $@"


######## Enclave Objects ########

MasterEnclave/MasterEnclave_t.c MasterEnclave/MasterEnclave_t.h: $(SGX_EDGER8R) MasterEnclave/MasterEnclave.edl
	@cd MasterEnclave && $(SGX_EDGER8R) --use-prefix --trusted ../MasterEnclave/MasterEnclave.edl --search-path $(SGX_SDK)/include 
	@echo "GEN  =>  $@"

MasterEnclave/MasterEnclave_t.o: MasterEnclave/MasterEnclave_t.c
	@$(CC) $(Enclave_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

MasterEnclave/%.o: MasterEnclave/%.cpp MasterEnclave/MasterEnclave_t.h
	@$(CXX) $(Enclave_Cxx_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

MasterEnclave.so: MasterEnclave/MasterEnclave_t.o $(Enclave_Cpp_Objects_1) $(Trust_Lib_Name)
	@$(CXX) -g MasterEnclave/MasterEnclave_t.o $(Enclave_Cpp_Objects_1) -o $@ $(MasterEnclave_Link_Flags)
	@echo "LINK =>  $@"

$(Enclave_Name_1): MasterEnclave.so
	@$(SGX_ENCLAVE_SIGNER) sign -key MasterEnclave/MasterEnclave_private.pem -enclave MasterEnclave.so -out $@ -config MasterEnclave/MasterEnclave.config.xml
	@echo "SIGN =>  $@"

GrapheneEnclave/GrapheneEnclave_t.c: $(SGX_EDGER8R) GrapheneEnclave/GrapheneEnclave.edl
	@cd GrapheneEnclave && $(SGX_EDGER8R)  --use-prefix --trusted ../GrapheneEnclave/GrapheneEnclave.edl --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

GrapheneEnclave/GrapheneEnclave_t.o: GrapheneEnclave/GrapheneEnclave_t.c
	@$(CC) $(Enclave_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

GrapheneEnclave/%.o: GrapheneEnclave/%.cpp
	@$(CXX) $(Enclave_Cxx_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

GrapheneEnclave.so: GrapheneEnclave/GrapheneEnclave_t.o $(Enclave_Cpp_Objects_2) $(Trust_Lib_Name)
	@$(CXX) -g GrapheneEnclave/GrapheneEnclave_t.o $(Enclave_Cpp_Objects_2) -o $@ $(GrapheneEnclave_Link_Flags)
	@echo "LINK =>  $@"

$(Enclave_Name_2): GrapheneEnclave.so
	@$(SGX_ENCLAVE_SIGNER) sign -key GrapheneEnclave/GrapheneEnclave_private.pem -enclave GrapheneEnclave.so -out $@ -config GrapheneEnclave/GrapheneEnclave.config.xml
	@echo "SIGN =>  $@"

######## Clean ########
.PHONY: clean

clean:
	@rm -rf .config_* $(App_Name) *.so *.a App/*.o MasterEnclave/*.o MasterEnclave/*_t.* MasterEnclave/*_u.* GrapheneEnclave/*.o GrapheneEnclave/*_t.* GrapheneEnclave/*_u.* LocalAttestationCode/*.o Untrusted_LocalAttestation/*.o LocalAttestationCode/*_t.*

