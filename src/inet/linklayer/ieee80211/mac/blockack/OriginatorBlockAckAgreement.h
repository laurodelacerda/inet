//
// Copyright (C) 2016 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see http://www.gnu.org/licenses/.
// 

#ifndef INET_ORIGINATORBLOCKACKAGREEMENT_H
#define INET_ORIGINATORBLOCKACKAGREEMENT_H

#include "inet/linklayer/ieee80211/mac/common/SequenceControlField.h"

namespace inet {
namespace ieee80211 {

class OriginatorBlockAckAgreementHandler;

class OriginatorBlockAckAgreement
{
    protected:
        MACAddress receiverAddr = MACAddress::UNSPECIFIED_ADDRESS;
        Tid tid = -1;
        int numSentBaPolicyFrames = 0;
        SequenceNumber startingSequenceNumber = -1;
        int bufferSize = -1;
        bool isAMsduSupported = false;
        bool isDelayedBlockAckPolicySupported = false;
        bool isAddbaResponseReceived = false;
        simtime_t blockAckTimeoutValue = -1;
        simtime_t expirationTime = -1;

    public:
        OriginatorBlockAckAgreement(MACAddress receiverAddr, Tid tid, SequenceNumber startingSequenceNumber, int bufferSize, bool isAMsduSupported, bool isDelayedBlockAckPolicySupported) :
            receiverAddr(receiverAddr),
            tid(tid),
            startingSequenceNumber(startingSequenceNumber),
            bufferSize(bufferSize),
            isAMsduSupported(isAMsduSupported),
            isDelayedBlockAckPolicySupported(isDelayedBlockAckPolicySupported)
        {
            renewExpirationTime();
        }

        virtual int getBufferSize() const { return bufferSize; }
        virtual SequenceNumber getStartingSequenceNumber() { return startingSequenceNumber; }
        virtual bool getIsAddbaResponseReceived() const { return isAddbaResponseReceived; }
        virtual bool getIsAMsduSupported() const { return isAMsduSupported; }
        virtual bool getIsDelayedBlockAckPolicySupported() const { return isDelayedBlockAckPolicySupported; }
        virtual MACAddress getReceiverAddr() const { return receiverAddr; }
        virtual Tid getTid() const { return tid; }
        virtual const simtime_t getBlockAckTimeoutValue() const  { return blockAckTimeoutValue; }
        virtual int getNumSentBaPolicyFrames() const { return numSentBaPolicyFrames; }

        virtual void setBufferSize(int bufferSize) { this->bufferSize = bufferSize; }
        virtual void setIsAddbaResponseReceived(bool isAddbaResponseReceived) { this->isAddbaResponseReceived = isAddbaResponseReceived; }
        virtual void setIsAMsduSupported(bool isAMsduSupported) { this->isAMsduSupported = isAMsduSupported; }
        virtual void setIsDelayedBlockAckPolicySupported(bool isDelayedBlockAckPolicySupported) { this->isDelayedBlockAckPolicySupported = isDelayedBlockAckPolicySupported; }
        virtual void setBlockAckTimeoutValue(const simtime_t blockAckTimeoutValue) { this->blockAckTimeoutValue = blockAckTimeoutValue; }

        virtual void baPolicyFrameSent() { numSentBaPolicyFrames++; }
        virtual void renewExpirationTime() { expirationTime = blockAckTimeoutValue == 0 ? SIMTIME_MAX : simTime() + blockAckTimeoutValue; }
        virtual simtime_t getExpirationTime() { return expirationTime; }
};

} /* namespace ieee80211 */
} /* namespace inet */

#endif // ifndef __INET_ORIGINATORBLOCKACKAGREEMENT_H
