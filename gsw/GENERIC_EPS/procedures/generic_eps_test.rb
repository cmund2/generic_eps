require 'cosmos'
require 'cosmos/script'
require 'mission_lib.rb'

class EPS_LPT < Cosmos::Test
  def setup
    
  end

  def test_lpt
    start("tests/generic_eps_lpt_test.rb")
  end

  def teardown

  end
end

class EPS_CPT < Cosmos::Test
  def setup
      
  end

  def test_cpt
    start("tests/generic_eps_cpt_test.rb")
  end

  def teardown

  end
end

class Generic_eps_Test < Cosmos::TestSuite
  def initialize
      super()
      add_test('EPS_CPT')
      add_test('EPS_LPT')
  end

  def setup
  end
  
  def teardown
  end
end