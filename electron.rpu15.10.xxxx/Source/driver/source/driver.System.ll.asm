; ----------------------------------------------------------------------------
; Access to the resources of TI TMS320F2833x DSP family .
;
; @author    Sergey Baigudin, sergey@baigudin.software
; ----------------------------------------------------------------------------
        .c28_amode

        .def  _eallow__6SystemSFv
        .def  _dallow__6SystemSFv
        
        .asg  _eallow__6SystemSFv, m_eallow
        .asg  _dallow__6SystemSFv, m_dallow

; ----------------------------------------------------------------------------
; Enables access to protected space.
; ----------------------------------------------------------------------------
        .text
m_eallow:
        eallow
        lretr
        
; ----------------------------------------------------------------------------
; Disables access to protected space.
; ----------------------------------------------------------------------------
        .text
m_dallow:        
        edis
        lretr
        
