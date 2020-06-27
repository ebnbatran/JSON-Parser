#include "utility.hpp"

namespace JSON {

    namespace Utility {

        long double ctold(const char &c) {
            switch (c) {
                case '0': return (long double)0.0; break;
                case '1': return (long double)1.0; break;
                case '2': return (long double)2.0; break;
                case '3': return (long double)3.0; break;
                case '4': return (long double)4.0; break;
                case '5': return (long double)5.0; break;
                case '6': return (long double)6.0; break;
                case '7': return (long double)7.0; break;
                case '8': return (long double)8.0; break;
                case '9': return (long double)9.0; break;
                default: return (long double)NAN; break;                
            }
        }

        long long ctoll(const char &c) {
            switch (c) {
                case '0': return (long long)0; break;
                case '1': return (long long)1; break;
                case '2': return (long long)2; break;
                case '3': return (long long)3; break;
                case '4': return (long long)4; break;
                case '5': return (long long)5; break;
                case '6': return (long long)6; break;
                case '7': return (long long)7; break;
                case '8': return (long long)8; break;
                case '9': return (long long)9; break;
                default: return (long long)NAN; break;                
            }
        }

    };
};