/*
 * common-structs.h
 *
 *  Created on: Jul 8, 2014
 *      Author: kentux
 */

#ifndef COMMON_STRUCTS_H_
#define COMMON_STRUCTS_H_


  struct LongPositionVector {
    uint64_t gnAddr;
    uint32_t Ts;
    uint32_t Lat;
    uint32_t Long;
    uint16_t Speed;
    uint16_t Heading;
    uint16_t Alt;
    uint8_t TimeAcc;
    uint8_t PosAcc;
    uint8_t SpeedAcc;
    uint8_t HeadingAcc;
    uint8_t AltAcc;
  };

  struct ShortPositionVector {
        uint64_t gnAddr;
        uint32_t Lat;
        uint32_t Long;
  };

  struct geoAreaPos
  {
          uint32_t lat;
          uint32_t lon;
  };


#endif /* COMMON_STRUCTS_H_ */
