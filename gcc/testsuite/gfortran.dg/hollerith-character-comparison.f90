       ! { dg-options "-foracle-support" }

      program convert
      REAL*4 a
      INTEGER*4 b
      b = 1000
      print *, 4HJMAC.eq.4HJMAC
      print *, 4HJMAC.eq."JMAC"
      print *, 4HJMAC.eq."JMAN"
      print *, "JMAC".eq.4HJMAN
      print *, "AAAA".eq.5HAAAAA
      print *, "BBBBB".eq.5HBBBB
      end program

