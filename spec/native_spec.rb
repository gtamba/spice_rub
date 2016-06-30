require 'spec_helper'
               

# test SpiceRub::Native.furnsh, SpiceRub::Native.ktotal, SpiceRub::Native.kclear SpiceRub::Native.unload
#
# Supposed to be tests for furnsh, load, ktotal, kclear : These functions are direct dependencies of the tests in kernel_spec.rb 
#  Specific tests for the C accessors will be put in later
#

describe "SpiceRub::Native" do
  #let(:test_object) { Class.new { extend SpiceRub::Native } }

  #kernel loading function tests here

  describe "functions that inter-convert co-ordinate systems" do
    before(:all) do
      EXAMPLE_COORDINATES = {
                             :lat =>  [1.0, PI / 2 , 0.0], 
                             :rec =>  NMatrix.new([3,1], [0.0, 1, 0.0]) , 
                             :sph =>  [1.0, PI / 2, PI / 2],
                             :pgr =>  1, 
                             :rad =>  [1.0, PI / 2, 0.0]
                            }
    end

    context "when co-ordinates are rectangular" do
      describe ".reclat" do
        subject { SpiceRub::Native.reclat(EXAMPLE_COORDINATES[:rec]) }     
        
        it { is_expected.to ary_be_within(0.0000000001).of(EXAMPLE_COORDINATES[:lat]) }
      end

      describe ".recsph" do
        subject { SpiceRub::Native.recsph(EXAMPLE_COORDINATES[:rec]) }     
        
        it { is_expected.to ary_be_within(0.0000000001).of(EXAMPLE_COORDINATES[:sph]) }
      end
      
      describe ".recrad" do
        subject { SpiceRub::Native.recrad(EXAMPLE_COORDINATES[:rec]) }     
        
        it { is_expected.to ary_be_within(0.0000000001).of EXAMPLE_COORDINATES[:rad] }
      end

      describe ".recpgr" do
        let(:expected) { [90.0 * SpiceRub::Native.rpd, 45.0 * SpiceRub::Native.rpd, 300] }

        before do
          kernel_pool = SpiceRub::KernelPool.instance
          kernel_pool.clear! unless kernel_pool.empty?
          kernel_pool.load(TEST_PCK_KERNEL[1])
        end

        subject { SpiceRub::Native.recpgr(:mars, NMatrix.new([3,1], [  1.604650025e-13,
                                                                      -2.620678915e+03,
                                                                       2.592408909e+03 ]), 3396.19,  0.005886007555525526) 
                }     
        
        it { is_expected.to ary_be_within(0.000001).of expected }
      end      
    end

    context "when co-ordinates are spherical" do
      describe ".sphrec" do
        subject { SpiceRub::Native.sphrec(*EXAMPLE_COORDINATES[:sph]) }     
      
        it { is_expected.to be_within(0.0000000001).of EXAMPLE_COORDINATES[:rec] }
      end

      describe ".sphlat" do
        skip("to be implemented")
      end
      
      describe ".sphrad" do
        skip("to be implemented")
      end

      describe ".recpgr" do
        skip("to be implemented")
        #subject {SpiceRub::Native.recsph(EXAMPLE_COORDINATES[:rec])}     
        
        #it { is_expected.to be_within(0.0000000001).of EXAMPLE_COORDINATES[:sph] }
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

        subject { SpiceRub::Native.pgrrec(:mars, 90.0 * SpiceRub::Native.rpd, 45.0 * SpiceRub::Native.rpd, 300, 3396.19,  0.005886007555525526) }     
      
        it { is_expected.to be_within(0.0000000001).of expected }
      end

      describe ".pgrlat" do
        skip("to be implemented")
      end
      
      describe ".pgrrad" do
        skip("to be implemented")
      end

      describe ".pgrsph" do
        skip("to be implemented")     
      end    
    end

    context "when co-ordinates are latitudinal" do
      describe ".latrec" do
        subject { SpiceRub::Native.latrec(*EXAMPLE_COORDINATES[:lat]) }     
      
        it { is_expected.to be_within(0.0000000001).of EXAMPLE_COORDINATES[:rec] }
      end

      describe ".latpgr" do
        skip("to be implemented")
      end
      
      describe ".latrad" do
        skip("to be implemented")
      end

      describe ".latsph" do
        skip("to be implemented")      
      end       
    end
    
    context "when co-ordinates are range, right ascension, and declination" do
      describe ".radrec" do
        subject { SpiceRub::Native.latrec(*EXAMPLE_COORDINATES[:lat]) }     
      
        it { is_expected.to be_within(0.0000000001).of EXAMPLE_COORDINATES[:rec] }
      end

      describe ".radpgr" do
        skip("to be implemented")
      end
      
      describe ".radlat" do
        skip("to be implemented")
      end

      describe ".radsph" do
        skip("to be implemented")      
      end       
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
        subject { SpiceRub::Native.bodvrd(:earth, :RADII, 3) } 
        
        it { is_expected.to eq([3, [6378.1366, 6378.1366, 6356.7519]]) }
      end
      
      context "when argument is mars and radii" do
        subject { SpiceRub::Native.bodvrd(:mars, :RADII, 3) } 
        
        it { is_expected.to eq([3, [3396.19, 3396.19, 3376.2]]) }
      end 
    end

    describe ".lspcn" do
      context "when no aberration correction is specificed" do
        subject { SpiceRub::Native.lspcn(:earth, SpiceRub::Native.str2et("21 March 2005") , :none) }
         
        it { is_expected.to eq 0.008404415875550564}
      end

      context "when aberration correction is :lt" do
        subject { SpiceRub::Native.lspcn(:moon, SpiceRub::Native.str2et("9 October 2000") , :lt) }
      
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
          subject { SpiceRub::Native.getfov(instrument, 4, 32, 32) } 
        
          it { is_expected.to eq(FOV_SOLUTIONS[index]) }
        end
      end 
    end  
    
    #Contrived test, skipping steps deriving the first argument due to lack of features present
    describe ".phaseq" do
      context "When no aberration correction is specified" do
        subject { SpiceRub::Native.phaseq(SpiceRub::Native.str2et("10 APRIL 2010"), :moon, :sun, :earth, :none) }
        it { is_expected.to eq 2.2494537984650593 }
      end
      
      context "When aberration correction is :lt" do
        subject { SpiceRub::Native.phaseq(SpiceRub::Native.str2et("10 MARCH 2000") , :moon, :sun, :earth, :lt) }
        
        it { is_expected.to eq 2.31272690572559 }
      end    
      
      context "When aberration correction is :lt+s " do
        subject { SpiceRub::Native.phaseq(SpiceRub::Native.str2et("1 January 2005") , :moon, :sun, :mars, :"lt+s") }
        
        it { is_expected.to eq 0.6347433745513293 }
      end  
      
      context "When aberration correction is :cn" do
        subject { SpiceRub::Native.phaseq(SpiceRub::Native.str2et("2 MAY 2002") , :moon, :sun, :earth, :cn) }
        
        it { is_expected.to eq 1.1039486416913542 }
      end  
      
      context "When aberration correction is :cn+s " do
        subject { SpiceRub::Native.phaseq(SpiceRub::Native.str2et("5 APRIL 1999") , :moon, :sun, :mars, :"cn+s") }
        
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
        subject { SpiceRub::Native.subpnt("Intercept:  ellipsoid", :mars, SpiceRub::Native.str2et("2008 August 11 00:00:00") , :iau_mars, :"lt+s", :earth) } 
        
        it {is_expected.to eq SUBPNT_SOLUTIONS[0]}
      end  
      
      context "When near-point elipsoid without aberration correction" do
        subject { SpiceRub::Native.subpnt("Near point: ellipsoid", :mars, SpiceRub::Native.str2et("2008 August 11 00:00:00") , :iau_mars, :"none", :earth) } 
        
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
        subject { SpiceRub::Native.subslr("Intercept: ellipsoid", :mars, SpiceRub::Native.str2et("2008 May 15 00:00:00") , :iau_mars, :"lt+s", :moon) } 
        
        it {is_expected.to eq SUBSLR_SOLUTIONS[0]}
      end  
      
      context "When near-point elipsoid without aberration correction" do
        subject { SpiceRub::Native.subslr("Near point: ellipsoid", :mars, SpiceRub::Native.str2et("2008 May 15 00:00:00") , :iau_mars, :"none", :moon) } 
        
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
      
        subject { SpiceRub::Native.gfdist(:MOON, 
                                          :NONE, 
                                          :EARTH, 
                                          :<, 
                                          400000, 
                                          0, 
                                          SpiceRub::Native.spd, 
                                          100, 
                                          [SpiceRub::Native.str2et("2007 JAN 1"), 
                                           SpiceRub::Native.str2et("2007 APR 1")]
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
      
        subject { SpiceRub::Native.gfsntc(:EARTH, 
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
                                          90 *SpiceRub::Native.spd, 100, 
                                          [SpiceRub::Native.str2et("2007 JAN 1"), 
                                           SpiceRub::Native.str2et("2008 JAN 1")])
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
      
        subject { SpiceRub::Native.gfsep(:MOON,
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
                                         6  * SpiceRub::Native.spd, 
                                         40, 
                                         [SpiceRub::Native.str2et("2007 JAN 1"), 
                                          SpiceRub::Native.str2et("2008 APR 1")])[0,4]
                } 

        it { is_expected.to ary_be_within(0.0000001).of(expected) }
      end
    end

    describe ".gfoclt" do
      context "when no aberration correction is specified" do
        
        let(:expected) {[
                         [61632614.20334693, 61637750.32880422]
                        ]}
      
        subject { SpiceRub::Native.gfoclt(:any, 
                                          :MOON, 
                                          :Ellipsoid, 
                                          :IAU_MOON, 
                                          :Sun, 
                                          :Ellipsoid, 
                                          :iau_sun, 
                                          :lt, 
                                          :earth, 
                                          180.0, 
                                          [SpiceRub::Native.str2et("2001 DEC 01 00:00:00 TDB"), 
                                           SpiceRub::Native.str2et("2002 JAN 01 00:00:00 TDB")])
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
          subject { SpiceRub::Native.timout(435443.342, "YYYY Mon DD, HR:MN:SC ::UTC", 40) }

          it { is_expected.to eq "2000 Jan 06, 12:56:19" }
        end
        
        context "When picture is MON DD,YYYY  HR:MN:SC.#### (TDB) ::TDB" do
          subject { SpiceRub::Native.timout(435443.342, "MON DD,YYYY  HR:MN:SC.#### (TDB) ::TDB", 40)}

          it { is_expected.to eq "JAN 06,2000  12:57:23.3420 (TDB)" }
        end
      end

      describe ".str2et" do
        subject { SpiceRub::Native.str2et("Nov 10 2000") }

        it { is_expected.to be_within(0.00000001).of 27086464.182655092 }
      end
      
      describe ".sce2c" do
        subject { SpiceRub::Native.sce2c(-77, 10000) }

        it { is_expected.to be_within(0.00000001).of 38768325676.97447 }
      end
     
      describe ".sctiks" do
        subject { SpiceRub::Native.sctiks(-77, "      0:01:000") }

        it { is_expected.to eq 80.0 }
      end
      
      describe ".scencd" do
        subject { SpiceRub::Native.scencd(-77, "0:01:001") }

        it { is_expected.to eq 88.0 }
      end
      
      describe ".scs2e" do
        subject { SpiceRub::Native.scs2e(-77, "11389.29.768") }
        
        it { is_expected.to be_within(0.00000001).of -322368174.21305406 }
      end
      
      describe ".scdecd" do
        subject { SpiceRub::Native.scdecd(-77, 11389, 40) }
        
        it { is_expected.to eq "1/00000001:51:3:5" }
      end    

      describe ".sct2e" do
        let(:sclkdp) { SpiceRub::Native.scencd(-77, "1/ 1900000:00:00") }

        subject { SpiceRub::Native.sct2e(-77, sclkdp) }
        
        it { is_expected.to eq -207792586.16380078 }
      end


    end
  end
end
  

