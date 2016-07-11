require 'spec_helper'
               
# TODO
# test spice.furnsh, spice.ktotal, spice.kclear spice.unload
#
# Supposed to be tests for furnsh, load, ktotal, kclear : These functions are direct dependencies of the tests in kernel_spec.rb 
#  Specific tests for the C accessors will be put in later
#

describe "SpiceRub::Native" do
  #Global alias to the native namespace
  let(:spice) { SpiceRub::Native }

  describe "functions that inter-convert co-ordinate systems" do
    before(:all) do
      EXAMPLE_COORDINATES = {
                             :lat =>  [1.0, PI / 2 , 0.0], 
                             :rec =>  NMatrix.new([3,1], [0.0, 1, 0.0]) , 
                             :sph =>  [1.0, PI / 2, PI / 2],
                             :rad =>  [1.0, PI / 2, 0.0],
                            }
    end

    context "when co-ordinates are rectangular" do
      describe ".reclat" do
        subject { spice.reclat(EXAMPLE_COORDINATES[:rec]) }     
        
        it { is_expected.to ary_be_within(0.0000000001).of(EXAMPLE_COORDINATES[:lat]) }
      end

      describe ".recsph" do
        subject { spice.recsph(EXAMPLE_COORDINATES[:rec]) }     
        
        it { is_expected.to ary_be_within(0.0000000001).of(EXAMPLE_COORDINATES[:sph]) }
      end
      
      describe ".recrad" do
        subject { spice.recrad(EXAMPLE_COORDINATES[:rec]) }     
        
        it { is_expected.to ary_be_within(0.0000000001).of EXAMPLE_COORDINATES[:rad] }
      end
      
      describe '.reccyl' do
        skip("pending implementation")
      end

      describe ".recpgr" do
        let(:expected) { [90.0 * spice.rpd, 45.0 * spice.rpd, 300] }

        before do
          kernel_pool = SpiceRub::KernelPool.instance
          kernel_pool.clear! unless kernel_pool.empty?
          kernel_pool.load(TEST_PCK_KERNEL[1])
        end

        subject { spice.recpgr(:mars, NMatrix.new([3,1], [  1.604650025e-13,
                                                            -2.620678915e+03,
                                                             2.592408909e+03 ]), 3396.19,  0.005886007555525526) 
                }     
        
        it { is_expected.to ary_be_within(0.000001).of expected }
      end      
      
      describe ".recgeo" do
        let(:expected) { [0.0, 90.0 * spice.rpd, -6356.583800130925] }

        subject { SpiceRub::Native.recgeo(NMatrix.new([3,1], 0.0), 6378.2064, 1.0 / 294.9787) }

        it { is_expected.to ary_be_within(0.000001).of expected }
      end
    end

    context "when co-ordinates are spherical" do
      describe ".sphrec" do
        subject { spice.sphrec(*EXAMPLE_COORDINATES[:sph]) }     
      
        it { is_expected.to be_within(0.0000000001).of EXAMPLE_COORDINATES[:rec] }
      end

      describe ".sphlat" do
        subject { spice.sphlat(*EXAMPLE_COORDINATES[:sph]) }

        it { is_expected.to ary_be_within(0.0000000001).of EXAMPLE_COORDINATES[:lat] }
      end
      
      describe ".sphcyl" do
        skip("to be implemented")
      end
    end

    context "when co-ordinates are planetographic" do
      describe ".pgrrec" do
        let(:expected) { NMatrix.new([3,1], [ 1.604650025e-13,
                                             -2.620678915e+03,
                                              2.592408909e+03 ] ) }
        before do
          kernel_pool = SpiceRub::KernelPool.instance
          kernel_pool.clear! unless kernel_pool.empty?
          kernel_pool.load(TEST_PCK_KERNEL[1])
        end          

        subject { spice.pgrrec(:mars, 90.0 * spice.rpd, 45.0 * spice.rpd, 300, 3396.19,  0.005886007555525526) }     
      
        it { is_expected.to be_within(0.0000000001).of expected }
      end  
    end

    context "when co-ordinates are latitudinal" do
      describe ".latrec" do
        subject { spice.latrec(*EXAMPLE_COORDINATES[:lat]) }     
      
        it { is_expected.to be_within(0.0000000001).of EXAMPLE_COORDINATES[:rec] }
      end

      describe ".latsph" do
        subject { spice.latsph(*EXAMPLE_COORDINATES[:lat]) }

        it { is_expected.to ary_be_within(0.0000000001).of EXAMPLE_COORDINATES[:sph]}      
      end       
      
      describe ".latcyl" do
        skip("to be implemented")
      end
    end
    
    context "when co-ordinates are range, right ascension, and declination" do
      describe ".radrec" do
        subject { spice.latrec(*EXAMPLE_COORDINATES[:lat]) }     
      
        it { is_expected.to be_within(0.0000000001).of EXAMPLE_COORDINATES[:rec] }
      end
    end    
  end
  
  context "when co-ordinates are geodetic" do
    describe ".georec" do
      let(:expected) { NMatrix.new([3,1], 0.0) }

      subject { SpiceRub::Native.georec(*[0.0, 90.0 * spice.rpd, -6356.583800130925], 6378.2064, 1.0 / 294.9787) }

      it { is_expected.to be_within(0.000001).of expected }
    end
        
  end

 
  describe "Functions that calculate various geometry measures using Kernel Data" do
    before(:all) do
        kernel_pool = SpiceRub::KernelPool.instance
          kernel_pool.clear! unless kernel_pool.empty?

        kernel_pool.load(TEST_PCK_KERNEL[1])
        kernel_pool.load(TEST_SPK_KERNEL) 
        kernel_pool.load(TEST_TLS_KERNEL)
        kernel_pool.load(TEST_IK_KERNEL)
    end
     
    describe ".bodvrd" do
      context "when argument is earth and radii" do
        subject { spice.bodvrd(:earth, :RADII, 3) } 
        
        it { is_expected.to eq([3, [6378.1366, 6378.1366, 6356.7519]]) }
      end
      
      context "when argument is mars and radii" do
        subject { spice.bodvrd(:mars, :RADII, 3) } 
        
        it { is_expected.to eq([3, [3396.19, 3396.19, 3376.2]]) }
      end 
    end

    describe ".lspcn" do
      context "when no aberration correction is specificed" do
        subject { spice.lspcn(:earth, spice.str2et("21 March 2005") , :none) }
         
        it { is_expected.to eq 0.008404415875550564}
      end

      context "when aberration correction is :lt" do
        subject { spice.lspcn(:moon, spice.str2et("9 October 2000") , :lt) }
      
        it { is_expected.to eq 1.4120962928065335}    
      end
    end

    describe ".getfov" do
      before(:all) do   
        FOV_SOLUTIONS = [
                     [
                      :CIRCLE, 
                      :SC999_INST001, 
                       NMatrix.new([3,1] , [0.0, 0.0, 1.0]), 
                      [NMatrix.new([3,1] , [0.08715574274765817, 0.0, 0.9961946980917455])],          
                      1
                     ] ,
                    
                     [
                      :ELLIPSE, 
                      :SC999_INST002,
                       NMatrix.new([3,1] , [1.0, 0.0, 0.0]), 
                      [
                       NMatrix.new([3,1] , [1.0, 0.0, 0.01745506]),  
                       NMatrix.new([3,1] , [1.0, 0.03492077, 0.0]), 
                      ],
                      2
                     ]
                    ]
                   
      end  
    
      TEST_INSTRUMENTS.each_with_index do |instrument, index|
        context "When instrument is #{instrument}" do
          subject { spice.getfov(instrument, 4, 32, 32) } 
        
          it { is_expected.to eq(FOV_SOLUTIONS[index]) }
        end
      end 
    end  
    
    #Contrived test, skipping steps deriving the first argument due to lack of features present
    describe ".phaseq" do
      context "When no aberration correction is specified" do
        subject { spice.phaseq(spice.str2et("10 APRIL 2010"), :moon, :sun, :earth, :none) }
        it { is_expected.to eq 2.2494537984650593 }
      end
      
      context "When aberration correction is :lt" do
        subject { spice.phaseq(spice.str2et("10 MARCH 2000") , :moon, :sun, :earth, :lt) }
        
        it { is_expected.to eq 2.31272690572559 }
      end    
      
      context "When aberration correction is :lt+s " do
        subject { spice.phaseq(spice.str2et("1 January 2005") , :moon, :sun, :mars, :"lt+s") }
        
        it { is_expected.to eq 0.6347433745513293 }
      end  
      
      context "When aberration correction is :cn" do
        subject { spice.phaseq(spice.str2et("2 MAY 2002") , :moon, :sun, :earth, :cn) }
        
        it { is_expected.to eq 1.1039486416913542 }
      end  
      
      context "When aberration correction is :cn+s " do
        subject { spice.phaseq(spice.str2et("5 APRIL 1999") , :moon, :sun, :mars, :"cn+s") }
        
        it { is_expected.to eq 2.6980870180829344 }
      end    
    end    

    describe ".sincpt" do
      skip("Function arguments dependent on SPICE functions that haven't been ported yet")
    end
      
    describe ".subpnt" do
      before(:all) do
        SUBPNT_SOLUTIONS = [
                            [
                             NMatrix.new([3,1], [296228138.09674716, -103750996.4499227, -153051054.03665805]),
                             NMatrix.new([3,1], [-2877.7318453657645, 1007.897319919223, 1486.826656636653]),
                             271683700.38023823
                            ],
                            
                            [ 
                             NMatrix.new([3,1],  [286674556.0707712, -127776213.52619617, -153046264.06300175]),
                             NMatrix.new([3,1],  [-2791.335574189609, 1244.1504932961989, 1472.7126181867986]),
                             271684865.183022 
                            ], 
                           ]
      end                     
  
      context "When intercept elipsoid with aberration correction lt + s" do
        subject { spice.subpnt("Intercept:  ellipsoid", :mars, spice.str2et("2008 August 11 00:00:00") , :iau_mars, :"lt+s", :earth) } 
        
        it {is_expected.to eq SUBPNT_SOLUTIONS[0]}
      end  
      
      context "When near-point elipsoid without aberration correction" do
        subject { spice.subpnt("Near point: ellipsoid", :mars, spice.str2et("2008 August 11 00:00:00") , :iau_mars, :"none", :earth) } 
        
        it {is_expected.to eq SUBPNT_SOLUTIONS[1]}
      end      
    end
    
    describe ".subslr" do
      before(:all) do
        SUBSLR_SOLUTIONS = [
                            [
                             NMatrix.new([3,1], [1611.1004244521614, 2651.589497401206, 1373.0060563159932]),
                             NMatrix.new([3,1], [-231966109.4496343, -100107205.64104867, -68252882.26676077]),
                             264080792.23976696
                            ],
                            
                            [ 
                             NMatrix.new([3,1],  [1774.9826754233334, 2552.0818214898377, 1359.5836003926045]),
                             NMatrix.new([3,1],  [-237692657.23841992, -85588608.37469248, -68259040.09187411]),
                             264081665.18525535 
                            ], 
                           ]
      end                     
  
      context "When intercept elipsoid with aberration correction lt + s" do
        subject { spice.subslr("Intercept: ellipsoid", :mars, spice.str2et("2008 May 15 00:00:00") , :iau_mars, :"lt+s", :moon) } 
        
        it {is_expected.to eq SUBSLR_SOLUTIONS[0]}
      end  
      
      context "When near-point elipsoid without aberration correction" do
        subject { spice.subslr("Near point: ellipsoid", :mars, spice.str2et("2008 May 15 00:00:00") , :iau_mars, :"none", :moon) } 
        
        it {is_expected.to eq SUBSLR_SOLUTIONS[1]}
      end      
    end
    
    describe ".subslr" do
      skip("pending")  
    end   
  end
  
  describe "Functions that return time interval windows for certain target observer constraints" do
    before(:all) do
        kernel_pool = SpiceRub::KernelPool.instance
        kernel_pool.clear! unless kernel_pool.empty?
        
        kernel_pool.load(TEST_TLS_KERNEL)
        kernel_pool.load(TEST_FRAME_KERNEL)
        kernel_pool.load(TEST_PCK_KERNEL[1])
        kernel_pool.load(TEST_SPK_KERNEL) 
    end    
    
    describe ".gfdist" do
      context "when no aberration correction is specified" do
        let(:expected) {[
                         [220881665.1839181, 221487067.6618968],
                         [221942267.9377619, 223844555.32055432],
                         [224371861.82920635, 226153225.2280659],
                         [226807478.4829023, 228480838.18623024]
                        ]}
      
        subject { spice.gfdist(:MOON, 
                                          :NONE, 
                                          :EARTH, 
                                          :<, 
                                          400000, 
                                          0, 
                                          spice.spd, 
                                          100, 
                                          [spice.str2et("2007 JAN 1"), 
                                           spice.str2et("2007 APR 1")]
                                         )
                } 

        it { is_expected.to ary_be_within(0.0000001).of(expected) }
      end
    end
    
    describe ".gfsntc" do
      context "when no aberration correction is specified" do
        
        let(:expected) {[
                         [227707285.52730322, 227707285.52730322],
                         [243812799.60698172, 243812799.60698172]
                        ]}
      
        subject { spice.gfsntc(:EARTH, 
                                          :IAU_EARTH, 
                                          :Ellipsoid, 
                                          :NONE, 
                                          :SUN, 
                                          :SEM, 
                                          NMatrix.new([3,1], [1.0,0,0]), 
                                          :LATITUDINAL, 
                                          :LATITUDE, 
                                          :"=", 
                                          0, 
                                          0, 
                                          90 *spice.spd, 100, 
                                          [spice.str2et("2007 JAN 1"), 
                                           spice.str2et("2008 JAN 1")])
                } 

        it { is_expected.to ary_be_within(0.0000001).of(expected) }
      end
    end

    describe ".gfsep" do
      context "when no aberration correction is specified" do
        
        let(:expected) {[
                         [221786480.22448748, 221786480.22448748],
                         [223047821.03743732, 223047821.03743732],
                         [224354993.44196665, 224354993.44196665],
                         [225595098.96333015, 225595098.96333015]
                        ]}
      
        subject { spice.gfsep(:MOON,
                                         :SPHERE, 
                                         :NULL, 
                                         :EARTH, 
                                         :SPHERE, 
                                         :NULL, 
                                         :NONE, 
                                         :SUN, 
                                         :LOCMAX, 
                                         0, 
                                         0, 
                                         6 * spice.spd, 
                                         40, 
                                         [spice.str2et("2007 JAN 1"), 
                                          spice.str2et("2008 APR 1")])[0,4]
                } 

        it { is_expected.to ary_be_within(0.0000001).of(expected) }
      end
    end

    describe ".gfoclt" do
      context "when no aberration correction is specified" do
        
        let(:expected) {[
                         [61632614.20334693, 61637750.32880422]
                        ]}
      
        subject { spice.gfoclt(:any, 
                                          :MOON, 
                                          :Ellipsoid, 
                                          :IAU_MOON, 
                                          :Sun, 
                                          :Ellipsoid, 
                                          :iau_sun, 
                                          :lt, 
                                          :earth, 
                                          180.0, 
                                          [spice.str2et("2001 DEC 01 00:00:00 TDB"), 
                                           spice.str2et("2002 JAN 01 00:00:00 TDB")])
                }

        it { is_expected.to ary_be_within(0.0000001).of(expected) }
      end
    end

    describe ".gftfov" do
      skip("Examples require additional kernels")
    end    
    
    describe ".gfrfov" do
      skip("Examples require additional kernels")
    end 
  
    describe "functions that convert time or time strings into different encodings" do
      before(:all) do
        kernel_pool = SpiceRub::KernelPool.instance
        kernel_pool.clear! unless kernel_pool.empty?
        kernel_pool.load(TEST_TLS_KERNEL)
        kernel_pool.load(TEST_SCLK_KERNEL)
      end

      describe ".timout" do
        context "When picture is YYYY Mon DD, HR:MN:SC ::UTC" do
          subject { spice.timout(435443.342, "YYYY Mon DD, HR:MN:SC ::UTC", 40) }

          it { is_expected.to eq "2000 Jan 06, 12:56:19" }
        end
        
        context "When picture is MON DD,YYYY  HR:MN:SC.#### (TDB) ::TDB" do
          subject { spice.timout(435443.342, "MON DD,YYYY  HR:MN:SC.#### (TDB) ::TDB", 40)}

          it { is_expected.to eq "JAN 06,2000  12:57:23.3420 (TDB)" }
        end
      end

      describe ".str2et" do
        subject { spice.str2et("Nov 10 2000") }

        it { is_expected.to be_within(0.00000001).of 27086464.182655092 }
      end
      
      describe ".sce2c" do
        subject { spice.sce2c(-77, 10000) }

        it { is_expected.to be_within(0.00000001).of 38768325676.97447 }
      end
     
      describe ".sctiks" do
        subject { spice.sctiks(-77, "      0:01:000") }

        it { is_expected.to eq 80.0 }
      end
      
      describe ".scencd" do
        subject { spice.scencd(-77, "0:01:001") }

        it { is_expected.to eq 88.0 }
      end
      
      describe ".scs2e" do
        subject { spice.scs2e(-77, "11389.29.768") }
        
        it { is_expected.to be_within(0.00000001).of -322368174.21305406 }
      end
      
      describe ".scdecd" do
        subject { spice.scdecd(-77, 11389, 40) }
        
        it { is_expected.to eq "1/00000001:51:3:5" }
      end    

      describe ".sct2e" do
        let(:sclkdp) { spice.scencd(-77, "1/ 1900000:00:00") }

        subject { spice.sct2e(-77, sclkdp) }
        
        it { is_expected.to eq -207792586.16380078 }
      end
    end
  end  
  
  describe "Functions that perform ephemerides based calculations using kernel data" do
    before(:all) do
      kernel_pool = SpiceRub::KernelPool.instance
      kernel_pool.clear! unless kernel_pool.empty?
      kernel_pool.load(TEST_PCK_KERNEL[1])
      kernel_pool.load(TEST_TLS_KERNEL)
      kernel_pool.load(TEST_SPK_KERNEL)
    end

    describe ".spkpos" do
      let(:expected) { [ NMatrix.new([3,1], [-97579460.22494915, -111325884.19041583, -53609500.96053864]) , 525.182681546206 ] }
      
      subject { spice.spkpos(:MOON, spice.str2et("2006 JAN 31 01:00"), :J2000, :NONE, :MARS) }
      
      it { is_expected.to ary_be_within(0.00000001).of expected }
    end

    describe ".spkezr" do
      let(:expected) { [ NMatrix.new([6,1], [-97579460.22494915, -111325884.19041583, -53609500.96053864, 191941265.18476546, 0, 0]) , 525.182681546206 ] }
      
      subject { spice.spkezr(:MOON, spice.str2et("2006 JAN 31 01:00"), :J2000, :NONE, :MARS) }
      
      it { is_expected.to ary_be_within(0.00000001).of expected }
    end
    
    describe ".sxform" do
      let(:expected) { NMatrix.new( [6,6], 
            [
              -0.181552128786035,      -0.9833808415169495,    -0.0009721474557003636,   0.0,                     0.0,                   0.0,
               0.9833813063908512,     -0.18155221353605164,   -1.0875961047432348e-06,  0.0,                     0.0,                   0.0,
              -0.00017542600129312876, -0.0009561890903792413,  0.9999995274639591,      0.0,                     0.0,                   0.0,
              -7.170926203563374e-05,   1.3238990046996728e-05, 3.080545126542364e-12,  -0.181552128786035,      -0.9833808415169495,   -0.0009721474557003636,
              -1.3238996230012214e-05, -7.170929593423952e-05,  6.892766900121194e-15,   0.9833813063908512,     -0.18155221353605164,  -1.0875961047432348e-06,
              -6.972585576496953e-08,   1.2795296388856552e-08, 2.99475301861348e-15,   -0.00017542600129312876, -0.0009561890903792413, 0.9999995274639591
            ])
      }
      
      subject { spice.sxform(:IAU_EARTH, :J2000, spice.str2et('January 1, 1990')) }
      
      it { is_expected.to be_within(0.00000001).of expected }
    end

    describe ".pxform" do
      let(:expected) { NMatrix.new( [3,3], 
            [
             -0.181552128786035,      -0.9833808415169495,   -0.0009721474557003636,
              0.9833813063908512,     -0.18155221353605164,  -1.0875961047432348e-06,
             -0.00017542600129312876, -0.0009561890903792413, 0.9999995274639591     
            ])
      }
      
      subject { spice.pxform(:IAU_EARTH, :J2000, spice.str2et('January 1, 1990')) }
      
      it { is_expected.to be_within(0.00000001).of expected }
    end

    describe ".pxfrm2" do
      let(:expected) { NMatrix.new( [3,3], 
            [   -0.8189920214285545,    -0.5738046002067962,     0.0005912849471995538,
                 0.5738047003181147,    -0.8189921647321643,    -4.0234320689515344e-07,
                 0.00048448860526348445, 0.00033895256605412914, 0.9999998251909594
            ]) 
      }
      
      subject { spice.pxfrm2(:IAU_EARTH, :J2000, spice.str2et("2006 JAN 31 01:00"), spice.str2et("2007 JAN 31 01:00")) }
      
      it { is_expected.to be_within(0.00000001).of expected }
    end


  end
end
  

