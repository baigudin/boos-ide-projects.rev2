/**
 * TI TMS320F2833x Analog-to-Digital Converter registers.
 *
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_ADC_REGISTER_HPP_
#define DRIVER_ADC_REGISTER_HPP_

#include "driver.Types.hpp"

/**
 * Analog-to-Digital Converter registers.
 */
struct AdcRegister
{

public:

  /**
   * Default configuration addresses.
   */
  static const uint32 ADDRESS = 0x00007100;
  
  /** 
   * Constructor.
   */  
  AdcRegister() :
    ctrl1   (),
    ctrl2   (),
    maxconv (),
    aseqsr  (),
    ctrl3   (),
    st      (),
    refsel  (),
    offtrim (){
  }
  
  /** 
   * Destructor.
   */    
 ~AdcRegister(){}    
  
  /**
   * Operator new.
   *
   * @param size unused.
   * @param ptr  address of memory.
   * @return address of memory.
   */     
  void* operator new(size_t, uint32 ptr)
  {
    return reinterpret_cast<void*>(ptr);
  }
  
  /**
   * ADC Control Register 1.
   */
  union Ctrl1
  {
    Ctrl1(){}
    Ctrl1(uint16 v){val = v;}          
   ~Ctrl1(){}    
   
    uint16 val;
    struct Val
    {
      uint16          : 4;
      uint16 seqCasc  : 1;
      uint16 seqOvrd  : 1;
      uint16 contRun  : 1;
      uint16 cps      : 1;
      uint16 acqPs    : 4;
      uint16 susmod   : 2;
      uint16 reset    : 1;
      uint16          : 1;
    } bit;
  } ctrl1;  

  /**
   * ADC Control Register 2.
   */
  union Ctrl2
  {
    Ctrl2(){}
    Ctrl2(uint16 v){val = v;}          
   ~Ctrl2(){}    
   
    uint16 val;
    struct Val
    {
      uint16 ePwmSocbSeq2 : 1;
      uint16              : 1;
      uint16 intModSeq2   : 1;
      uint16 intEnaSeq2   : 1;
      uint16              : 1;
      uint16 socSeq2      : 1;
      uint16 rstSeq2      : 1;
      uint16 extSocSeq1   : 1;
      uint16 epwmSocaSeq1 : 1;
      uint16              : 1;
      uint16 intModSeq1   : 1;
      uint16 intEnaSeq1   : 1;
      uint16              : 1;
      uint16 socSeq1      : 1;
      uint16 rstSeq1      : 1;
      uint16 epwmSocbSeq  : 1;
    } bit;
  } ctrl2;

  /**
   * ADC Maximum Conversion Channels Register.
   */
  union Maxconv
  {
    Maxconv(){}
    Maxconv(uint16 v){val = v;}          
   ~Maxconv(){}    
   
    uint16 val;
    struct Val
    {
      uint16 maxConv1 : 4;
      uint16 maxConv2 : 3;
      uint16          : 9;

    } bit;
  } maxconv;

  /**
   * ADC Channel Select Sequencing Control Register 1-4.
   */
  union Chselseq
  {
    Chselseq(){}
    Chselseq(uint16 v){val = v;}          
   ~Chselseq(){}    
   
    uint16 val;
    struct Val
    {
      uint16 conv0 : 4;
      uint16 conv1 : 4; 
      uint16 conv2 : 4;
      uint16 conv3 : 4; 
    } bit;
  } chselseq[4];

  /**
   * ADC Auto-Sequence Status Register.
   */
  union Aseqsr
  {
    Aseqsr(){}
    Aseqsr(uint16 v){val = v;}          
   ~Aseqsr(){}    
   
    uint16 val;
    struct Val
    {
      uint16 seq1State : 4;
      uint16 seq2State : 3;
      uint16           : 1;
      uint16 seqCntr   : 4;
      uint16           : 4;
    } bit;
  } aseqsr;

  /**
   * ADC Conversion Result Buffer Register 0-15.
   */
  union Result
  {
    Result(){}
    Result(uint16 v){val = v;}          
   ~Result(){}    
   
    uint16 val;
    struct Val
    {
      uint16     : 4;
      uint16 d0  : 1;
      uint16 d1  : 1;
      uint16 d2  : 1;
      uint16 d3  : 1;
      uint16 d4  : 1;
      uint16 d5  : 1;
      uint16 d6  : 1;
      uint16 d7  : 1;
      uint16 d8  : 1;
      uint16 d9  : 1;
      uint16 d10 : 1;
      uint16 d11 : 1;
    } bit;
  } result[16];

  /**
   * ADC Control Register 3.
   */
  union Ctrl3
  {
    Ctrl3(){}
    Ctrl3(uint16 v){val = v;}          
   ~Ctrl3(){}    
   
    uint16 val;
    struct Val
    {
      uint16 smodeSel  : 1;
      uint16 adcclkps  : 4;
      uint16 adcpwdn   : 1;
      uint16 adcbgrfdn : 2;
      uint16           : 8;
    } bit;
  } ctrl3;

  /**
   * ADC Status and Flag Register.
   */
  union St
  {
    St(){}
    St(uint16 v){val = v;}          
   ~St(){}    
   
    uint16 val;
    struct Val
    {
      uint16 intSeq1      : 1;
      uint16 intSeq2      : 1;
      uint16 seq1Bsy      : 1;
      uint16 seq2Bsy      : 1;
      uint16 intSeq1Clr   : 1;
      uint16 intSeq2Clr   : 1;
      uint16 eosBuf1      : 1;
      uint16 eosBuf2      : 1;
      uint16              : 8;
    } bit;
  } st;


private:        

  uint16 space0_[2];
  
public:

  /**
   * ADC Reference Select Register.
   */
  union Refsel
  {
    Refsel(){}
    Refsel(uint16 v){val = v;}          
   ~Refsel(){}    
   
    uint16 val;
    struct Val
    {
      uint16        : 14;
      uint16 refSel : 2;
    } bit;
  } refsel;

  /**
   * ADC Offset Trim Register.
   */
  union Offtrim
  {
    Offtrim(){}
    Offtrim(uint16 v){val = v;}          
   ~Offtrim(){}    
   
    uint16 val;
    struct Val
    {
      uint16 offsetTrim : 8;    
      uint16            : 8;

    } bit;
  } offtrim;

private:        

  uint16 space1_[2];
  
public:

};

/**
 * Analog-to-Digital Converter registers for DMA and CPU fast access.
 */
struct AdcDmaRegister
{

public:
  
  /**
   * Remapped configuration addresses.
   */
  static const uint32 ADDRESS = 0x00000B00;
  
  /** 
   * Constructor.
   */  
  AdcDmaRegister(){}
  
  /** 
   * Destructor.
   */    
 ~AdcDmaRegister(){}    
  
  /**
   * Operator new.
   *
   * @param size unused.
   * @param ptr  address of memory.
   * @return address of memory.
   */     
  void* operator new(size_t, uint32 ptr)
  {
    return reinterpret_cast<void*>(ptr);
  }

  /**
   * ADC Conversion Result Buffer Register 0-15.
   */
  union Result
  {
    Result(){}
    Result(uint16 v){val = v;}          
   ~Result(){}    
   
    uint16 val;
    struct Val
    {
      uint16     : 4;
      uint16 d0  : 1;
      uint16 d1  : 1;
      uint16 d2  : 1;
      uint16 d3  : 1;
      uint16 d4  : 1;
      uint16 d5  : 1;
      uint16 d6  : 1;
      uint16 d7  : 1;
      uint16 d8  : 1;
      uint16 d9  : 1;
      uint16 d10 : 1;
      uint16 d11 : 1;
    } bit;
  } result[16];

};

#endif // DRIVER_ADC_REGISTER_HPP_
