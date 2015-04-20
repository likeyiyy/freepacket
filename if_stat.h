
#ifndef _IF_STAT_H_
#define _IF_STAT_H_

#ifdef INAC_CARD_SIDE
#include <stdint.h>
#endif

/* enum mac registers need to read. */
enum _e_mac_reg {

  /* GBE */
  E_MPIPE_GBE_BEGIN        = 0,
  E_MPIPE_GBE_TX_BEGIN     = E_MPIPE_GBE_BEGIN,
  E_MPIPE_GBE_OCTETS_TX_LO = E_MPIPE_GBE_TX_BEGIN,
  E_MPIPE_GBE_OCTETS_TX_HI,
  E_MPIPE_GBE_FRAMES_TX,
  E_MPIPE_GBE_BCST_FRAMES_TX,
  E_MPIPE_GBE_MCST_FRAMES_TX,
  E_MPIPE_GBE_PAUSE_FRAMES_TX,
  E_MPIPE_GBE_64_BYTE_FRAMES_TX,
  E_MPIPE_GBE_65_TO_127_BYTE_FRAMES_TX,
  E_MPIPE_GBE_128_TO_255_BYTE_FRAMES_TX,
  E_MPIPE_GBE_256_TO_511_BYTE_FRAMES_TX,
  E_MPIPE_GBE_512_TO_1023_BYTE_FRAMES_TX,
  E_MPIPE_GBE_1024_TO_1518_BYTE_FRAMES_TX,
  E_MPIPE_GBE_GREATER_THAN_1518_BYTE_FRAMES_TX,
  E_MPIPE_GBE_TX_UNDER_RUNS,
  E_MPIPE_GBE_SINGLE_COLLISION_FRAMES,
  E_MPIPE_GBE_MULTIPLE_COLLISION_FRAMES,
  E_MPIPE_GBE_EXCESSIVE_COLLISIONS,
  E_MPIPE_GBE_LATE_COLLISIONS,
  E_MPIPE_GBE_DEFERRED_TRANSMISSION_FRAMES,
  E_MPIPE_GBE_CARRIER_SENSE_ERRORS,
  E_MPIPE_GBE_RX_BEGIN,
  E_MPIPE_GBE_OCTETS_RX_LO = E_MPIPE_GBE_RX_BEGIN,
  E_MPIPE_GBE_OCTETS_RX_HI,
  E_MPIPE_GBE_FRAMES_RX,
  E_MPIPE_GBE_BCST_FRAMES_RX,
  E_MPIPE_GBE_MCST_FRAMES_RX,
  E_MPIPE_GBE_PAUSE_FRAMES_RX,
  E_MPIPE_GBE_64_BYTE_FRAMES_RX,
  E_MPIPE_GBE_65_TO_127_BYTE_FRAMES_RX,
  E_MPIPE_GBE_128_TO_255_BYTE_FRAMES_RX,
  E_MPIPE_GBE_256_TO_511_BYTE_FRAMES_RX,
  E_MPIPE_GBE_512_TO_1023_BYTE_FRAMES_RX,
  E_MPIPE_GBE_1024_TO_1518_BYTE_FRAMES_RX,
  E_MPIPE_GBE_GREATER_THAN_1518_BYTE_FRAMES_RX,
  E_MPIPE_GBE_UNDERSIZE_FRAMES_RX,
  E_MPIPE_GBE_OVERSIZE_FRAMES_RX,
  E_MPIPE_GBE_JABBERS_RX,
  E_MPIPE_GBE_FRAME_CHECK_SEQUENCE_ERRORS,
  E_MPIPE_GBE_LENGTH_FRAME_ERRORS,
  E_MPIPE_GBE_RECEIVE_SYMBOL_ERRORS,
  E_MPIPE_GBE_ALIGNMENT_ERRORS,
  E_MPIPE_GBE_RECEIVE_OVERRUNS,
  E_MPIPE_GBE_IP_HEADER_CHECKSUM_ERRORS,
  E_MPIPE_GBE_TCP_CHECKSUM_ERRORS,
  E_MPIPE_GBE_UDP_CHECKSUM_ERRORS,
  E_MPIPE_GBE_END,

  /* XAUI */
  E_MPIPE_XAUI_BEGIN = E_MPIPE_GBE_END,
  E_MPIPE_XAUI_OCTETS_TX_LO = E_MPIPE_XAUI_BEGIN,
  E_MPIPE_XAUI_OCTETS_TX_HI,
  E_MPIPE_XAUI_FRAMES_TX_LO,
  E_MPIPE_XAUI_FRAMES_TX_HI,
  E_MPIPE_XAUI_BCST_FRAMES_TX,
  E_MPIPE_XAUI_MCST_FRAMES_TX,
  E_MPIPE_XAUI_PAUSE_FRAMES_TX,
  E_MPIPE_XAUI_64_BYTE_FRAMES_TX,
  E_MPIPE_XAUI_65_TO_127_BYTE_FRAMES_TX,
  E_MPIPE_XAUI_128_TO_255_BYTE_FRAMES_TX,
  E_MPIPE_XAUI_256_TO_511_BYTE_FRAMES_TX,
  E_MPIPE_XAUI_512_TO_1023_BYTE_FRAMES_TX,
  E_MPIPE_XAUI_1024_TO_1518_BYTE_FRAMES_TX,
  E_MPIPE_XAUI_GREATER_THAN_1518_BYTE_FRAMES_TX,
  E_MPIPE_XAUI_TRANSMITTED_ERROR_FRAMES,
  E_MPIPE_XAUI_OCTETS_RX_LO,
  E_MPIPE_XAUI_OCTETS_RX_HI,
  E_MPIPE_XAUI_FRAMES_RX_LO,
  E_MPIPE_XAUI_FRAMES_RX_HI,
  E_MPIPE_XAUI_BCST_FRAMES_RX,
  E_MPIPE_XAUI_MCST_FRAMES_RX,
  E_MPIPE_XAUI_PAUSE_FRAMES_RX,
  E_MPIPE_XAUI_64_BYTE_FRAMES_RX,
  E_MPIPE_XAUI_65_TO_127_BYTE_FRAMES_RX,
  E_MPIPE_XAUI_128_TO_255_BYTE_FRAMES_RX,
  E_MPIPE_XAUI_256_TO_511_BYTE_FRAMES_RX,
  E_MPIPE_XAUI_512_TO_1023_BYTE_FRAMES_RX,
  E_MPIPE_XAUI_1024_TO_1518_BYTE_FRAMES_RX,
  E_MPIPE_XAUI_GREATER_THAN_1518_BYTE_FRAMES_RX,
  E_MPIPE_XAUI_SHORT_FRAMES_RX,
  E_MPIPE_XAUI_OVERSIZE_FRAMES_RX,
  E_MPIPE_XAUI_JABBERS_RX,
  E_MPIPE_XAUI_FRAME_CRC_ERRORS_RX,
  E_MPIPE_XAUI_LENGTH_FIELD_ERRORS_RX,
  E_MPIPE_XAUI_RECEIVE_SYMBOL_ERRORS_RX,
  E_MPIPE_XAUI_END
};


#define GBE_MAC(_IDX_)   (((_IDX_) >= 0) && ((_IDX_) < E_MPIPE_GBE_END))
#define XGBE_MAC(_IDX_)  (((_IDX_) >= E_MPIPE_XAUI_BEGIN)  &&   \
                          ((_IDX_) < E_MPIPE_XAUI_END))

typedef struct if_stats_s
{
	//uint64_t	rx_packets;		/* total packets received	*/
	uint64_t	tx_packets;		/* total packets transmitted	*/
	//uint64_t	rx_bytes;		/* total bytes received 	*/
	uint64_t	tx_bytes;		/* total bytes transmitted	*/
	//uint64_t	rx_errors;		/* bad packets received		*/
	uint64_t	tx_errors;		/* packet transmit problems	*/
	//uint64_t	rx_dropped;		/* no space in linux buffers	*/
	//uint64_t	rx_dropped_bytes;	/* no space in linux buffers	*/
	uint64_t	tx_dropped;		/* no space available in linux	*/
	uint64_t	rx_multicast;		/* multicast packets received	*/
	uint64_t	collisions;

	uint64_t 	rx_hits;

	uint64_t	rx_card_mac_packets; 
	uint64_t	rx_card_mac_bytes; 
	uint64_t	rx_card_packets; 
	uint64_t	rx_card_bytes; 
	uint64_t	rx_card_dropped; 
	uint64_t	rx_card_dropped_bytes;	
	uint64_t	rx_card_errors;  
	uint64_t	rx_card_error_bytes;
	/*all ports drop pkts*/
	uint64_t	rx_mpipe_dropped; 

	uint64_t	tx_card_packets; 
	uint64_t	tx_card_bytes; 
	uint64_t	tx_card_errors;  

	uint64_t	rx_other_errors; /*huge frame and not ip packet*/
	uint64_t	rx_other_error_bytes;

	uint64_t	dma_rx_packets;		/* card -> host */
	uint64_t	dma_rx_bytes;		
	uint64_t	dma_rx_drops;	
	uint64_t	dma_rx_errors;		

	uint32_t	link_status;
	uint32_t	link_speed;
} if_stats_t;

/** mPIPE statistics structure. These counters include all relevant
 *  events occurring on all links within the mPIPE shim. */
typedef struct
{
  /** Number of ingress packets dropped for any reason. */
  uint64_t ingress_drops;
  /** Number of ingress packets dropped because a buffer stack was empty. */
  uint64_t ingress_drops_no_buf;
  /** Number of ingress packets dropped or truncated due to lack of space in
   *  the iPkt buffer. */
  uint64_t ingress_drops_ipkt;
  /** Number of ingress packets dropped by the classifier or load balancer */
  uint64_t ingress_drops_cls_lb;
  /** Total number of ingress packets. */
  uint64_t ingress_packets;
  /** Total number of egress packets. */
  uint64_t egress_packets;
  /** Total number of ingress bytes. */
  uint64_t ingress_bytes;
  /** Total number of egress bytes. */
  uint64_t egress_bytes;
}
host_mpipe_stats_t;

typedef struct anti_attack_stat_s
{
	uint64_t	syn_drop;
	uint64_t	ack_drop;
	uint64_t	dns_drop;
}anti_attack_stat_t;

#ifdef INAC_HOST_DRV
static inline void if_stats_ntoh(if_stats_t *if_stat)
{
}
#endif

#endif /* _IF_STAT_H_ */

