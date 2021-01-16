//
//  2019, Daniel Probst, Reymond Group @ University of Bern
//
//   @@ All Rights Reserved @@
//  This file is part of the RDKit.
//  The contents are covered by the terms of the BSD license
//  which is included in the file license.txt, found at the root
//  of the RDKit source tree.
//

/*! \file MHFP.h

*/
#include <RDGeneral/export.h>
#ifndef RD_MHFPFPS_H
#define RD_MHFPFPS_H
#include <string>
#include <vector>
#include <GraphMol/ROMol.h>
#include <DataStructs/ExplicitBitVect.h>

class SparseBitVect;
namespace RDKit {
namespace MHFPFingerprints {
const std::string mhfpFingerprintVersion = "1.0.0";

namespace FNV {
  const uint32_t prime = 0x01000193;
  const uint32_t seed = 0x811C9DC5;

  //! A simple implementation of the Fowler–Noll–Vo hash function.
  inline uint32_t
  hash(const std::string& str, uint32_t hash = seed) {
    const unsigned char* ptr = (const unsigned char*)str.c_str();
    size_t len = str.length();

    while (len--)
      hash = (*ptr++ ^ hash) * prime;
    
    return hash;
  };
}

class RDKIT_FINGERPRINTS_EXPORT MHFPEncoder {
public:
  //! Constructor
  /*!
    \brief Construct a MHFPEncoder
   
    The MHFPEncoder class is instantieted with a given number of permutations
    and a seed. Fingerprints / minhashes created with a different number of
    permutations or a different seed are not compatible.
   
    \param n_permutations the number of permutations used to create hash functions.
              This will be the dimensionality of the resulting vector.
              Default: <tt>2048</tt>.
    \param seed a random seed. Default: <tt>42</tt>.
   */
  MHFPEncoder(unsigned int n_permutations = 2048,
        unsigned int seed = 42);
  
  /*!
    \brief Creates a MinHash from a vector of strings.
   
    This method is exposed in order to enable advanced usage of this MHFP 
    implementation such as customizing the properties that are hashed in order 
    to create an MHFP instance. In theory, any number of values that can be 
    represented  as strings can be minhashed. This method is called 
    by MHFPEncoder::Encode.
   
    \param vec a vector containg strings (e.g. the smiles shingling of a molecule).

    \returns the MinHash of the input.
   */
  std::vector<uint32_t>
  FromStringArray(const std::vector<std::string>& vec);

  /*!
    \brief Creates a MinHash from a list of unsigned integers.
   
    This method is exposed in order to enable advanced usage of this MHFP 
    implementation such as MinHashing a sparse array generated by another
    fingerprint (e.g. Morgan / ECFP).
   
    \param vec a vector containg unsigned integers.

    \returns the MinHash of the input.
   */
  std::vector<uint32_t>
  FromArray(const std::vector<uint32_t>& vec);

  /*!
    \brief Creates a molecular shingling based on circular substructures.
   
    A molecular shingling is a vector of SMILES that were extracted from and
    represent a molecule. This method extracts substructures centered at each
    atom of the molecule with different radii. A molecule with 10 atoms will
    generate <tt>10 * 3</tt> shingles when a radius of <tt>3</tt> is chosen.
   
    \param radius the maximum radius of the substructure that is generated at
          each atom. Default: <tt>3</tt>.
    \param rings whether the rings (SSSR) are extrected from the molecule and
           added to the shingling. Given the molecule 
           <tt>"C1CCCCCC1C(=O)C"</tt>, "<tt>C1CCCCCC1"</tt> would be added
           to the shingling. Default: <tt>true</tt>.
    \param isomeric whether the SMILES added to the shingling are isomeric.
            Default: <tt>false</tt>.
    \param kekulize whether the SMILES added to the shingling are kekulized.
            Default: <tt>true</tt>.
    \param min_radius the minimum radius that is used to extract n-grams.
            Default: <tt>1</tt>.

    \returns the shingling of a molecule.
   */
  std::vector<std::string>
  CreateShingling(const ROMol& mol,
          unsigned char radius = 3,
          bool rings = true,
          bool isomeric = false,
          bool kekulize = true,
          unsigned char min_radius = 1);

  //! \overload
  std::vector<std::string>
  CreateShingling(const std::string& smiles,
          unsigned char radius = 3,
          bool rings = true,
          bool isomeric = false,
          bool kekulize = true,
          unsigned char min_radius = 1);

  /*!
    \brief Creates a MinHash vector from a molecule.
   
    This methods is a wrapper around MHFPEncoder::CreateShingling and
    MHFPEncoder::FromStringArray. When a vector of molecules or SMILES is passed
    and RDKit was compiled with OpenMP, it is parallelized and will speed up by
    a factor of the number of cores.
   
    \param radius the maximum radius of the substructure that is generated at
          each atom. Default: <tt>3</tt>.
    \param rings whether the rings (SSSR) are extrected from the molecule and
           added to the shingling. Given the molecule 
           <tt>"C1CCCCCC1C(=O)C"</tt>, "<tt>C1CCCCCC1"</tt> would be added
           to the shingling. Default: <tt>true</tt>.
    \param isomeric whether the SMILES added to the shingling are isomeric.
            Default: <tt>false</tt>.
    \param kekulize whether the SMILES added to the shingling are kekulized.
            Default: <tt>true</tt>.
    \param min_radius the minimum radius that is used to extract n-grams.
            Default: <tt>1</tt>.

    \returns the MHFP fingerprint.
   */
  std::vector<uint32_t>
  Encode(ROMol& mol,
       unsigned char radius = 3,
       bool rings = true,
       bool isomeric = false,
       bool kekulize = true,
       unsigned char min_radius = 1);

  //! \overload
  std::vector<std::vector<uint32_t>>
  Encode(std::vector<ROMol>& mols,
       unsigned char radius = 3,
       bool rings = true,
       bool isomeric = false,
       bool kekulize = true,
       unsigned char min_radius = 1);

  //! \overload
  std::vector<uint32_t>
  Encode(std::string& smiles,
       unsigned char radius = 3,
       bool rings = true,
       bool isomeric = false,
       bool kekulize = true,
       unsigned char min_radius = 1);

  //! \overload
  std::vector<std::vector<uint32_t>>
  Encode(std::vector<std::string>& smiles,
       unsigned char radius = 3,
       bool rings = true,
       bool isomeric = false,
       bool kekulize = true,
       unsigned char min_radius = 1);


  /*!
    \brief Creates a binary fingerprint based on circular sub-SMILES.
   
    Creates a binary fingerprint similar to ECFP. However, instead of using
    a Morgan-style hashing, circular n-grams (sub-SMILES) are created, hashed
    directly and folded.
   
    \param radius the maximum radius of the substructure that is generated at
          each atom. Default: <tt>3</tt>.
    \param rings whether the rings (SSSR) are extrected from the molecule and
           added to the shingling. Given the molecule 
           <tt>"C1CCCCCC1C(=O)C"</tt>, "<tt>C1CCCCCC1"</tt> would be added
           to the shingling. Default: <tt>true</tt>.
    \param isomeric whether the SMILES added to the shingling are isomeric.
            Default: <tt>false</tt>.
    \param kekulize whether the SMILES added to the shingling are kekulized.
            Default: <tt>true</tt>.
    \param min_radius the minimum radius that is used to extract n-grams.
            Default: <tt>1</tt>.
    \param length the length into which the fingerprint is folded.
            Default: <tt>2048</tt>.

    \returns the SECFP fingerprint.
   */
  ExplicitBitVect
  EncodeSECFP(ROMol& mol,
       unsigned char radius = 3,
       bool rings = true,
       bool isomeric = false,
       bool kekulize = true,
       unsigned char min_radius = 1,
       size_t length = 2048);

  //! \overload
  std::vector<ExplicitBitVect>
  EncodeSECFP(std::vector<ROMol>& mols,
       unsigned char radius = 3,
       bool rings = true,
       bool isomeric = false,
       bool kekulize = true,
       unsigned char min_radius = 1,
       size_t length = 2048);

  //! \overload
  ExplicitBitVect
  EncodeSECFP(std::string& smiles,
       unsigned char radius = 3,
       bool rings = true,
       bool isomeric = false,
       bool kekulize = true,
       unsigned char min_radius = 1,
       size_t length = 2048);

  //! \overload
  std::vector<ExplicitBitVect>
  EncodeSECFP(std::vector<std::string>& smiles,
       unsigned char radius = 3,
       bool rings = true,
       bool isomeric = false,
       bool kekulize = true,
       unsigned char min_radius = 1,
       size_t length = 2048);

  /*!
    \brief Calculates the Jaccard / Tanimoto distance between two MHFP fingerprints.
   
    \param a an MHFP fingerprint vector.
    \param b an MHFP fingerprint vector.

    \returns the Jaccard / Tanimoto distance between the two fingerprints.
   */
  static double
  Distance(const std::vector<uint32_t>& a, 
       const std::vector<uint32_t>& b) {
    size_t matches = 0;

    for (size_t i = 0; i < a.size(); i++)
      if (a[i] == b[i])
        matches++;

    return matches / (double)a.size();
  }

private:
  //! The fastest mod implementation. 
  uint64_t
  FastMod(const uint64_t input, const uint64_t ceil) {
    return input >= ceil ? input % ceil : input;
  }

  ExplicitBitVect
  Fold(const std::vector<uint32_t>& vec, uint32_t length = 2048) {
    ExplicitBitVect ebv(length);
    for (size_t i = 0; i < vec.size(); i++)
      ebv.setBit(vec[i] % length);
    return ebv;
  }

  std::vector<uint32_t>
  HashShingling(std::vector<std::string> vec) {
    std::vector<uint32_t> result(vec.size());
    for (size_t i = 0; i < vec.size(); i++)
      result[i] = FNV::hash(vec[i]);
    return result;
  }

  unsigned int n_permutations_, seed_;
  uint64_t prime_ = 2305843009213693951UL;
  uint32_t max_hash_ = 4294967295;
  std::vector<uint32_t> perms_a_;
  std::vector<uint32_t> perms_b_;
};

}  // namespace MHFPFingerprints
}  // namespace RDKit

#endif