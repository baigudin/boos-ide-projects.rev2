; ----------------------------------------------------------------------------
; Analog-to-Digital Converter of TI TMS320F2833x DSP family.
;
; @author    Sergey Baigudin, sergey@baigudin.software
; ----------------------------------------------------------------------------
        .c28_amode

        .def  _ADC_cal
        .def  _calibrate__13AdcControllerSFv
        
        .asg  _calibrate__13AdcControllerSFv,  m_calibrate
        .asg  _ADC_cal,                        m_adc_cal
        .asg  "0x711C",                        ADCREFSEL_LOC
        
; ----------------------------------------------------------------------------
; Copies device specific calibration data.
; ----------------------------------------------------------------------------
        .text
m_calibrate:
        lcr             m_adc_cal
        lretr        

; ----------------------------------------------------------------------------
; Copies device specific calibration data into ADCREFSEL and ADCOFFTRIM regs
;
; This is the ADC cal routine. This routine is programmed into reserved memory 
; by the factory. 0xAAAA and 0xBBBB are place holders. The actual values 
; programmed by TI are device specific.
;
; The ADC clock must be enabled before calling this function.
; ----------------------------------------------------------------------------
        .sect ".adc_cal"
m_adc_cal:
        movw            dp,  #ADCREFSEL_LOC >> 6
        mov             @28, #0xaaaa            ; Actual value may not be 0xAAAA
        mov             @29, #0xbbbb            ; Actual value may not be 0xBBBB
        lretr
        
