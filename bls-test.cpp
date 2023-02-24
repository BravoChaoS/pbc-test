#include <libBLS.h>
#include <vector>
#include <fstream>

#include <libff/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include <libff/algebra/exponentiation/exponentiation.hpp>


int main() {
    size_t num_signed = 3;
    size_t num_all = 4;
    std::vector <std::vector<libff::alt_bn128_Fr>> secret_shares_all; // matrix of all secret shares
    std::vector <std::vector<libff::alt_bn128_G2>> public_shares_all; //// matrix of all public shares
    std::vector <DKGTEWrapper> dkgs; // instances of DKGTEWrapper for each participant
    std::vector <TEPrivateKeyShare> skeys; // private keys of participants
    std::vector <TEPublicKeyShare> pkeys;  // public keys of participants

    for (size_t i = 0; i < num_all; i++) {
        DKGTEWrapper dkg_wrap(num_signed, num_all);
        dkgs.push_back(dkg_wrap);

// create secret shares for each participant
        std::shared_ptr <std::vector<libff::alt_bn128_Fr>> secret_shares_ptr =
                dkg_wrap.createDKGSecretShares();

// create public shares for each participant
        std::shared_ptr <std::vector<libff::alt_bn128_Fr>> public_shares_ptr =
                dkg_wrap.createDKGPublicShares();

        secret_shares_all.push_back(*secret_shares_ptr);
        public_shares_all.push_back(*public_shares_ptr);
    }

    for (size_t i = 0; i < num_all; i++)      // Verifying shares for each participant
        for (size_t j = 0; j < num_all; j++) {
            assert(dkgs.at(i).VerifyDKGShare(j, secret_shares_all.at(i).at(j),
                                             std::make_shared < std::vector <
                                             libff::alt_bn128_G2 >> (public_shares_all.at(i))));
        }
    std::vector <std::vector<libff::alt_bn128_Fr>> secret_key_shares;

    for (size_t i = 0; i < num_all; i++) {          // collect got secret shares in a vector
        std::vector <libff::alt_bn128_Fr> secret_key_contribution;
        for (size_t j = 0; j < num_all; j++) {
            secret_key_contribution.push_back(secret_shares_all.at(j).at(i));
        }
        secret_key_shares.push_back(secret_key_contribution);
    }

    for (size_t i = 0; i < num_all; i++) {
        TEPrivateKeyShare pkey_share = dkgs.at(i).CreateTEPrivateKeyShare(
                i + 1,
                std::make_shared < std::vector < libff::alt_bn128_Fr >> (
                        secret_key_shares.at(i)));
        skeys.push_back(pkey_share);
        pkeys.push_back(TEPublicKeyShare(pkey_share, num_signed, num_all));
    }

    TEPublicKey common_public = DKGTEWrapper::CreateTEPublicKey(
    std::make_shared < std::vector < std::vector < libff::alt_bn128_G2>>>(public_shares_all),
            num_signed,
            num_all);

    std::string message;    // Generating random message
    size_t msg_length = 64;
    for (size_t length = 0; length < msg_length; ++length) {
        message += char(rand_gen() % 128);
    }

    std::shared_ptr msg_ptr = std::make_shared<std::string>(message);
    libBLS::Ciphertext cypher = common_public.encrypt(msg_ptr);

// removing 1 random participant ( because only 3 of 4 will participate)
    size_t ind4del = rand_gen() % secret_shares_all.size();
    auto pos4del = secret_shares_all.begin();
    advance(pos4del, ind4del);
    secret_shares_all.erase(pos4del);
    auto pos2 = public_shares_all.begin();
    advance(pos2, ind4del);
    public_shares_all.erase(pos2);

    TEDecryptSet decr_set(num_signed, num_all);
    for (size_t i = 0; i < num_signed; i++) {
        libff::alt_bn128_G2 decrypt = skeys.at(i).decrypt(cypher);
        assert(pkeys.at(i).Verify(cypher, decrypt.el_));
        std::shared_ptr decr_ptr = std::make_shared<libff::alt_bn128_G2>(decrypt);
        decr_set.addDecrypt(skeys.at(i).getSignerIndex(), decr_ptr);
    }

    std::string message_decrypted = decr_set.merge(cypher);
}