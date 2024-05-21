#pragma once

#include "UDP/Address.h"
#include "UDP/Datagram.h"
#include "UDP/RangedInteger.h"
#include "UDP/Settings.h"

#include <chrono>
#include <vector>
#include <random>

#if BOUSKNET_ALLOW_NETWORK_SIMULATOR == BOUSKNET_SETTINGS_ENABLED
namespace Bousk
{
	namespace Network
	{
		namespace UDP
		{
			class Simulator
			{
			public:
				Simulator();

				void enable() { mEnabled = true; }
				void disable() { mEnabled = false; }
				bool isEnabled() const { return mEnabled; }
				void seed(unsigned int value);

				void setDuplicate(RangedInteger<1, 10> _duplicate) { mDuplicate = _duplicate; }
				void setLossRate(RangedInteger<0, 100> _loss) { mLoss = _loss; }
				void setDelay(RangedInteger<0, 250> _fixed, RangedInteger<0, 100> _random) { mFixedDelay = _fixed; mRandomDelay = _random; }

				void push(const Datagram& _datagram, const Address& _from);
				std::vector<std::pair<Datagram, Address>> poll();

			private:
				struct PendingDatagram
				{
					Datagram datagram;
					Address from;
					std::chrono::milliseconds receivedTime;
					std::chrono::milliseconds processTime;
				};
				std::vector<PendingDatagram> mPendingDatagrams;

				RangedInteger<1, 10> mDuplicate{ 3 };
				RangedInteger<0, 1000> mDuplicateRate{ 1 };
				std::default_random_engine mDuplicateGenerator;
				RangedInteger<0, 100> mLoss{ 5 };
				std::default_random_engine mLossGenerator;
				RangedInteger<0, 250> mFixedDelay{ 0 };
				RangedInteger<0, 100> mRandomDelay{ 50 };
				std::default_random_engine mRandomDelayGenerator;
				bool mEnabled{ false };
			};
		}
	}
}
#endif // BOUSKNET_ALLOW_NETWORK_SIMULATOR == BOUSKNET_SETTINGS_ENABLED