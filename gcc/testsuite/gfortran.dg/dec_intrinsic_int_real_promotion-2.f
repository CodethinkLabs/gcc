! { dg-do compile }
! { dg-options "-fdec" }
!
! Test promotion between integers and reals in intrinsic operations.
! These operations are: mod, modulo, dim, sign, min, max, minloc and
! maxloc.
!
      PROGRAM promotion_int_real
        REAL l/0.0/
        LOGICAL a_i
        LOGICAL*4 a2_i
        LOGICAL b_i
        LOGICAL*8 b2_i
        LOGICAL x_i
        LOGICAL y_i
        CHARACTER a_r
        CHARACTER*4 a2_r
        CHARACTER b_r
        CHARACTER*8 b2_r
        CHARACTER x_r
        CHARACTER y_r

        INTEGER m_i/0/
        REAL m_r/0.0/

        INTEGER md_i/0/
        REAL md_r/0.0/

        INTEGER d_i/0/
        REAL d_r/0.0/

        INTEGER s_i/0/
        REAL s_r/0.0/

        INTEGER mn_i/0/
        REAL mn_r/0.0/

        INTEGER mx_i/0/
        REAL mx_r/0.0/

        m_i = MOD(a_i, b_i)                     ! { dg-error "" }
        if (m_i .ne. 1) STOP 1
        m_i = MOD(a2_i, b2_i)                   ! { dg-error "" }
        if (m_i .ne. 1) STOP 2
        m_r = MOD(a_r, b_r)                     ! { dg-error "" }
        if (abs(m_r - 1.0) > 1.0D-6) STOP 3
        m_r = MOD(a2_r, b2_r)                   ! { dg-error "" }
        if (abs(m_r - 1.0) > 1.0D-6) STOP 4
        m_r = MOD(a_i, b_r)                     ! { dg-error "" }
        if (abs(m_r - 1.0) > 1.0D-6) STOP 5
        m_r = MOD(a_r, b_i)                     ! { dg-error "" }
        if (abs(m_r - 1.0) > 1.0D-6) STOP 6

        md_i = MODULO(a_i, b_i)                 ! { dg-error "" }
        if (md_i .ne. 1) STOP 7
        md_i = MODULO(a2_i, b2_i)               ! { dg-error "" }
        if (md_i .ne. 1) STOP 8
        md_r = MODULO(a_r, b_r)                 ! { dg-error "" }
        if (abs(md_r - 1.0) > 1.0D-6) STOP 9
        md_r = MODULO(a2_r, b2_r)               ! { dg-error "" }
        if (abs(md_r - 1.0) > 1.0D-6) STOP 10
        md_r = MODULO(a_i, b_r)                 ! { dg-error "" }
        if (abs(md_r - 1.0) > 1.0D-6) STOP 11
        md_r = MODULO(a_r, b_i)                 ! { dg-error "" }
        if (abs(md_r - 1.0) > 1.0D-6) STOP 12

        d_i = DIM(a_i, b_i)                     ! { dg-error "" }
        if (d_i .ne. 1) STOP 13
        d_i = DIM(a2_i, b2_i)                   ! { dg-error "" }
        if (d_i .ne. 1) STOP 14
        d_r = DIM(a_r, b_r)                     ! { dg-error "" }
        if (abs(d_r - 1.0) > 1.0D-6) STOP 15
        d_r = DIM(a2_r, b2_r)                   ! { dg-error "" }
        if (abs(d_r - 1.0) > 1.0D-6) STOP 16
        d_r = DIM(a_r, b_i)                     ! { dg-error "" }
        if (abs(d_r - 1.0) > 1.0D-6) STOP 17
        d_r = DIM(b_i, a_r)                     ! { dg-error "" }
        if (abs(d_r) > 1.0D-6) STOP 18

        s_i = SIGN(-a_i, b_i)                   ! { dg-error "" }
        if (s_i .ne. 4) STOP 19
        s_i = SIGN(-a2_i, b2_i)                 ! { dg-error "" }
        if (s_i .ne. 4) STOP 20
        s_r = SIGN(a_r, -b_r)                   ! { dg-error "" }
        if (abs(s_r - (-a_r)) > 1.0D-6) STOP 21 ! { dg-error "" }
        s_r = SIGN(a2_r, -b2_r)                 ! { dg-error "" }
        if (abs(s_r - (-a2_r)) > 1.0D-6) STOP 22 ! { dg-error "" }
        s_r = SIGN(a_r, -b_i)                   ! { dg-error "" }
        if (abs(s_r - (-a_r)) > 1.0D-6) STOP 23 ! { dg-error "" }
        s_r = SIGN(-a_i, b_r)                   ! { dg-error "" }
        if (abs(s_r - a_r) > 1.0D-6) STOP 24    ! { dg-error "" }

        mx_i = MAX(-a_i, -b_i, x_i, y_i)        ! { dg-error "" }
        if (mx_i .ne. x_i) STOP 25              ! { dg-error "" }
        mx_i = MAX(-a2_i, -b2_i, x_i, y_i)      ! { dg-error "" }
        if (mx_i .ne. x_i) STOP 26              ! { dg-error "" }
        mx_r = MAX(-a_r, -b_r, x_r, y_r)        ! { dg-error "" }
        if (abs(mx_r - x_r) > 1.0D-6) STOP 27   ! { dg-error "" }
        mx_r = MAX(-a_r, -b_r, x_r, y_r)        ! { dg-error "" }
        if (abs(mx_r - x_r) > 1.0D-6) STOP 28   ! { dg-error "" }
        mx_r = MAX(-a_i, -b_r, x_r, y_i)        ! { dg-error "" }
        if (abs(mx_r - x_r) > 1.0D-6) STOP 29   ! { dg-error "" }

        mn_i = MIN(-a_i, -b_i, x_i, y_i)        ! { dg-error "" }
        if (mn_i .ne. -a_i) STOP 31             ! { dg-error "" }
        mn_i = MIN(-a2_i, -b2_i, x_i, y_i)      ! { dg-error "" }
        if (mn_i .ne. -a2_i) STOP 32            ! { dg-error "" }
        mn_r = MIN(-a_r, -b_r, x_r, y_r)        ! { dg-error "" }
        if (abs(mn_r - (-a_r)) > 1.0D-6) STOP 33 ! { dg-error "" }
        mn_r = MIN(-a2_r, -b2_r, x_r, y_r)      ! { dg-error "" }
        if (abs(mn_r - (-a2_r)) > 1.0D-6) STOP 34 ! { dg-error "" }
        mn_r = MIN(-a_i, -b_r, x_r, y_i)        ! { dg-error "" }
        if (abs(mn_r - (-a_r)) > 1.0D-6) STOP 35 ! { dg-error "" }
      END PROGRAM
