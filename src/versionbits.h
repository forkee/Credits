// Copyright (c) 2009-2017 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Developers
// Copyright (c) 2014-2017 The Dash Core Developers
// Copyright (c) 2017 Credits Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef CREDITS_VERSIONBITS_H
#define CREDITS_VERSIONBITS_H

#include "chain.h"

#include <map>

/** What block version to use for new blocks (pre versionbits) */
static const int32_t VERSIONBITS_LAST_OLD_BLOCK_VERSION = 4;
/** What bits to set in version for versionbits blocks */
static const int32_t VERSIONBITS_TOP_BITS = 0x20000000UL;
/** What bitmask determines whether versionbits is in use */
static const int32_t VERSIONBITS_TOP_MASK = 0xE0000000UL;
/** Total bits available for versionbits */
static const int32_t VERSIONBITS_NUM_BITS = 29;

enum ThresholdState {
    THRESHOLD_DEFINED,
    THRESHOLD_STARTED,
    THRESHOLD_LOCKED_IN,
    THRESHOLD_ACTIVE,
    THRESHOLD_FAILED,
};

// A map that gives the state for blocks whose height is a multiple of Period().
// The map is indexed by the block's parent, however, so all keys in the map
// will either be NULL or a block with (height + 1) % Period() == 0.
typedef std::map<const CBlockIndex*, ThresholdState> ThresholdConditionCache;

struct BIP9DeploymentInfo {
    /** Deployment name */
    const char *name;
    /** Whether GBT clients can safely ignore this rule in simplified usage */
    bool gbt_force;
};

extern const struct BIP9DeploymentInfo VersionBitsDeploymentInfo[];

/**
 * Abstract class that implements BIP9-style threshold logic, and caches results.
 */
class AbstractThresholdConditionChecker {
protected:
    virtual bool Condition(const CBlockIndex* pindex, const Consensus::Params& params) const =0;
    virtual int64_t BeginTime(const Consensus::Params& params) const =0;
    virtual int64_t EndTime(const Consensus::Params& params) const =0;
    virtual int Period(const Consensus::Params& params) const =0;
    virtual int Threshold(const Consensus::Params& params) const =0;

public:
    // Note that the function below takes a pindexPrev as input: they compute information for block B based on its parent.
    ThresholdState GetStateFor(const CBlockIndex* pindexPrev, const Consensus::Params& params, ThresholdConditionCache& cache) const;
    int GetStateSinceHeightFor(const CBlockIndex* pindexPrev, const Consensus::Params& params, ThresholdConditionCache& cache) const;
};

struct VersionBitsCache
{
    ThresholdConditionCache caches[Consensus::MAX_VERSION_BITS_DEPLOYMENTS];

    void Clear();
};

ThresholdState VersionBitsState(const CBlockIndex* pindexPrev, const Consensus::Params& params, Consensus::DeploymentPos pos, VersionBitsCache& cache);
int VersionBitsStateSinceHeight(const CBlockIndex* pindexPrev, const Consensus::Params& params, Consensus::DeploymentPos pos, VersionBitsCache& cache);
uint32_t VersionBitsMask(const Consensus::Params& params, Consensus::DeploymentPos pos);

#endif // CREDITS_VERSIONBITS_H