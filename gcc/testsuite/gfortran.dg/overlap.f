! { dg-do run }
! { dg-options "-std=extra-legacy" }!
!     Legacy fortran compilers accept overlapping
!     non-equal initializers. The last DATA statement
!     should win the conflict.

      PROGRAM	OVERLAP
      CHARACTER*2 tickers(2,8)
      integer*4 max_sub
      parameter (max_sub = 3)
      character*2 sub1(2,max_sub),sub2(2,max_sub),
     .            sub3(2,max_sub)
      equivalence (tickers,sub1)
      equivalence (tickers(1,3),sub2)
      equivalence (tickers(1,5),sub3)

      DATA sub1   / 'AB',' E',  'CD',' C',  'EF','V ' /
      DATA sub3   / 'AB',' E',  'CD',' C',  'EF','V ' /
      DATA sub2   / 'AB',' E',  'CD',' C',  'EF','V ' /

      PRINT *, tickers(1,3)
      PRINT *, tickers(1,5)

      if(tickers(1,3) .ne. 'AB') then
         call abort
      end if
      if(tickers(1,5) .ne. 'EF') then
         call abort
      end if
      END PROGRAM


