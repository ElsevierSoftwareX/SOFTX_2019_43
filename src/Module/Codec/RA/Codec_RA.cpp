#include "Tools/Exception/exception.hpp"

#include "Factory/Tools/Interleaver/Interleaver_core.hpp"
#include "Factory/Module/Puncturer/Puncturer.hpp"

#include "Codec_RA.hpp"

using namespace aff3ct;
using namespace aff3ct::module;

template <typename B, typename Q>
Codec_RA<B,Q>
::Codec_RA(const factory::Encoder_RA::parameters &enc_params,
           const factory::Decoder_RA::parameters &dec_params)
: Codec     <B,Q>(enc_params.K, enc_params.N_cw, enc_params.N_cw, enc_params.tail_length, enc_params.n_frames),
  Codec_SIHO<B,Q>(enc_params.K, enc_params.N_cw, enc_params.N_cw, enc_params.tail_length, enc_params.n_frames)
{
	const std::string name = "Codec_RA";
	this->set_name(name);

	// ----------------------------------------------------------------------------------------------------- exceptions
	if (enc_params.K != dec_params.K)
	{
		std::stringstream message;
		message << "'enc_params.K' has to be equal to 'dec_params.K' ('enc_params.K' = " << enc_params.K
		        << ", 'dec_params.K' = " << dec_params.K << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	if (enc_params.N_cw != dec_params.N_cw)
	{
		std::stringstream message;
		message << "'enc_params.N_cw' has to be equal to 'dec_params.N_cw' ('enc_params.N_cw' = " << enc_params.N_cw
		        << ", 'dec_params.N_cw' = " << dec_params.N_cw << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	if (enc_params.n_frames != dec_params.n_frames)
	{
		std::stringstream message;
		message << "'enc_params.n_frames' has to be equal to 'dec_params.n_frames' ('enc_params.n_frames' = "
		        << enc_params.n_frames << ", 'dec_params.n_frames' = " << dec_params.n_frames << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	// ---------------------------------------------------------------------------------------------------- allocations
	factory::Puncturer::parameters pct_params;
	pct_params.type     = "NO";
	pct_params.K        = enc_params.K;
	pct_params.N        = enc_params.N_cw;
	pct_params.N_cw     = enc_params.N_cw;
	pct_params.n_frames = enc_params.n_frames;

	this->set_puncturer  (std::shared_ptr<Puncturer<B,Q>            >(factory::Puncturer::build<B,Q>(pct_params)));
	this->set_interleaver(std::shared_ptr<tools::Interleaver_core< >>(factory::Interleaver_core::build<>(*dec_params.itl->core)));

	try
	{
		std::shared_ptr<Encoder_RA<B>> enc(factory::Encoder_RA::build<B>(enc_params, this->get_interleaver_bit()));
		this->set_encoder(std::static_pointer_cast<Encoder<B>>(enc));
	}
	catch (tools::cannot_allocate const&)
	{
		this->set_encoder(std::shared_ptr<Encoder<B>>(factory::Encoder::build<B>(enc_params)));
	}

	this->set_decoder_siho(std::shared_ptr<Decoder_SIHO<B,Q>>(
		factory::Decoder_RA::build<B,Q>(dec_params, this->get_interleaver_llr(), this->get_encoder())));
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template class aff3ct::module::Codec_RA<B_8,Q_8>;
template class aff3ct::module::Codec_RA<B_16,Q_16>;
template class aff3ct::module::Codec_RA<B_32,Q_32>;
template class aff3ct::module::Codec_RA<B_64,Q_64>;
#else
template class aff3ct::module::Codec_RA<B,Q>;
#endif
// ==================================================================================== explicit template instantiation
