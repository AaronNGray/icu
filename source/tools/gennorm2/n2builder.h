// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2009-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  n2builder.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2009nov25
*   created by: Markus W. Scherer
*/

#ifndef __N2BUILDER_H__
#define __N2BUILDER_H__

#include "unicode/utypes.h"

#if !UCONFIG_NO_NORMALIZATION

#include "unicode/errorcode.h"
#include "unicode/unistr.h"
#include "normalizer2impl.h"  // for IX_COUNT
#include "toolutil.h"
#include "utrie2.h"
#include "norms.h"

U_NAMESPACE_BEGIN

extern UBool beVerbose, haveCopyright;

class Normalizer2DataBuilder {
public:
    Normalizer2DataBuilder(UErrorCode &errorCode);
    ~Normalizer2DataBuilder();

    enum OverrideHandling {
        OVERRIDE_NONE,
        OVERRIDE_ANY,
        OVERRIDE_PREVIOUS
    };

    void setOverrideHandling(OverrideHandling oh);

    enum Optimization {
        OPTIMIZE_NORMAL,
        OPTIMIZE_FAST
    };

    void setOptimization(Optimization opt) { optimization=opt; }

    void setCC(UChar32 c, uint8_t cc);
    void setOneWayMapping(UChar32 c, const UnicodeString &m);
    void setRoundTripMapping(UChar32 c, const UnicodeString &m);
    void removeMapping(UChar32 c);

    void setUnicodeVersion(const char *v);

    void writeBinaryFile(const char *filename);
    void writeCSourceFile(const char *filename);

private:
    friend class Norm16Writer;

    Normalizer2DataBuilder(const Normalizer2DataBuilder &other) = delete;
    Normalizer2DataBuilder &operator=(const Normalizer2DataBuilder &other) = delete;

    Norm *checkNormForMapping(Norm *p, UChar32 c);  // check for permitted overrides

    /**
     * A starter character with a mapping does not have a composition boundary after it
     * if the character itself combines-forward (which is tested by the caller of this function),
     * or it is deleted (mapped to the empty string),
     * or its mapping contains no starter,
     * or the last starter combines-forward.
     */
    UBool mappingHasCompBoundaryAfter(const BuilderReorderingBuffer &buffer) const;
    /** Returns TRUE if the mapping by itself recomposes, that is, it is not comp-normalized. */
    UBool mappingRecomposes(const BuilderReorderingBuffer &buffer) const;
    void postProcess(Norm &norm);

    void setSmallFCD(UChar32 c);
    int32_t getMinNoNoDelta() const {
        return indexes[Normalizer2Impl::IX_MIN_MAYBE_YES]-
            ((2*Normalizer2Impl::MAX_DELTA+1)<<Normalizer2Impl::DELTA_SHIFT);
    }
    void writeNorm16(UChar32 start, UChar32 end, Norm &norm);
    void setHangulData();
    void processData();

    Norms norms;

    int32_t phase;
    OverrideHandling overrideHandling;

    Optimization optimization;

    int32_t indexes[Normalizer2Impl::IX_COUNT];
    UTrie2 *norm16Trie;
    int32_t norm16TrieLength;
    UnicodeString extraData;
    uint8_t smallFCD[0x100];

    UVersionInfo unicodeVersion;
};

U_NAMESPACE_END

#endif // #if !UCONFIG_NO_NORMALIZATION

#endif  // __N2BUILDER_H__
