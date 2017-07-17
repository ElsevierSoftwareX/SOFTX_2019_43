#ifndef FACTORY_SIMULATION_EXIT_HPP_
#define FACTORY_SIMULATION_EXIT_HPP_

#include <string>
#include <thread>

#include "Tools/Arguments_reader.hpp"
#include "Tools/Factory/Module/Factory_source.hpp"
#include "Tools/Factory/Module/Factory_modem.hpp"
#include "Tools/Factory/Module/Factory_channel.hpp"
#include "Tools/Factory/Module/Factory_quantizer.hpp"
#include "Tools/Factory/Module/Factory_monitor.hpp"
#include "Tools/Factory/Module/Code/Factory_puncturer.hpp"
#include "Tools/Factory/Module/Code/Factory_encoder.hpp"
#include "Tools/Factory/Module/Code/Factory_decoder.hpp"
#include "Tools/Factory/Simulation/EXIT/Factory_terminal_EXIT.hpp"

#include "../Factory_simulation.hpp"

namespace aff3ct
{
namespace tools
{

struct Factory_simulation_EXIT : Factory_simulation
{
	struct parameters : Factory_simulation::parameters
	{
		virtual ~parameters() {}

		std::string snr_type   = "ES";
		float       sig_a_min  = 0.0f;
		float       sig_a_max  = 5.0f;
		float       sig_a_step = 0.5f;
		bool        debug      = false;
	};

	struct chain_parameters : Factory_simulation::chain_parameters
	{
		chain_parameters()
		{
			if(this->sim == nullptr)
				this->sim = new parameters();
		}

		virtual ~chain_parameters()
		{
			if(this->sim != nullptr)
				delete this->sim;

			this->sim = nullptr;
		}

		Factory_simulation_EXIT::parameters *sim = nullptr;
		Factory_source         ::parameters  src;
		Factory_modem          ::parameters  mdm;
		Factory_channel        ::parameters  chn;
		Factory_puncturer      ::parameters *pct = nullptr;
		Factory_encoder        ::parameters *enc = nullptr;
		Factory_decoder        ::parameters *dec = nullptr;
		Factory_terminal_EXIT  ::parameters  ter;
	};

	static void build_args(Arguments_reader::arg_map &req_args, Arguments_reader::arg_map &opt_args);
	static void store_args(const Arguments_reader& ar, parameters &params);
	static void group_args(Arguments_reader::arg_grp& ar);

	static void header(params_list& head_sim, const parameters& params);
};

}
}

#endif /* FACTORY_SIMULATION_EXIT_HPP_ */