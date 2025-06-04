// ======================================================================
// \title  Generic_eps.hpp
// \author jstar
// \brief  hpp file for Generic_eps component implementation class
// ======================================================================

#ifndef Components_Generic_eps_HPP
#define Components_Generic_eps_HPP

#include "eps_src/Generic_epsComponentAc.hpp"

namespace Components {

  class Generic_eps :
    public Generic_epsComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct Generic_eps object
      Generic_eps(
          const char* const compName //!< The component name
      );

      //! Destroy Generic_eps object
      ~Generic_eps();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for commands
      // ----------------------------------------------------------------------

      //! Handler implementation for command TODO
      //!
      //! TODO
      // void TODO_cmdHandler(
      //     FwOpcodeType opCode, //!< The opcode
      //     U32 cmdSeq //!< The command sequence number
      // ) override;

      void REQUEST_HOUSEKEEPING_cmdHandler(
        FwOpcodeType opCode, 
        U32 cmdSeq
      ) override;

  };

}

#endif
