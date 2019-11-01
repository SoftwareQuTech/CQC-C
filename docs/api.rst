CQC C Library 
=============

The library is defined in `cqc.h` and implemented in `cqc.c`.

All the CQC headers are defined as `__packed__` structs in `cqc.h`.

CQC connection state is kept in the following structure:

.. code-block:: c

    typedef struct
    {
        int sockfd;       // connection to CQC backend
        uint16_t app_id;  // application ID
    } cqc_ctx;

All CQC library function return an `int` that takes one of the following values:

.. code-block:: c

    enum cqc_lib_rc
    {
        CQC_LIB_OK = 0,
        CQC_LIB_ERR = -1,
    };

The EPR commands also return an entanglement header:

.. code-block:: c

    typedef struct
    {
        uint32_t node_A;            /* IP of this node */
        uint16_t port_A;            /* Port of this node */
        uint16_t app_id_A;          /* App ID of this node */
        uint32_t node_B;            /* IP of other node */
        uint16_t port_B;            /* Port of other node */
        uint16_t app_id_B;          /* App ID of other node */
        uint32_t id_AB;             /* Entanglement identifier */
        uint64_t timestamp;         /* Creation time */
        uint64_t tog;               /* Time of goodness */
        uint16_t goodness;          /* Goodness parameter */
        uint8_t df;                 /* Directionality flag creation */
        uint8_t unused;             /* Not used - align */
    } __attribute__((__packed__)) entanglementHeader;

The following functions are available in the library. To execute a specific command, you need to use the right message type as defined in `cqc.h`. These can also be found in this documentation https://softwarequtech.github.io/CQC-Python/interface.html:

.. code-block:: c

    /*
     * cqc_init
     *
     * Create and return a cqc_ctx handle.
     *
     * Arguments:
     * app_id    ID to use for this application
     */
    cqc_ctx *cqc_init(uint16_t app_id)

    /*
     * cqc_connect
     *
     * Connect to CQC backend.
     *
     * Arguments:
     *
     * hostname    hostname of the CQC backend
     * portno      port of the CQC backend
     */
    int cqc_connect(cqc_ctx *cqc, char *hostname, int portno)

    /*
     * cqc_close
     *
     * Disconnect from the backend.
     */
    void cqc_close(cqc_ctx *cqc)

    /*
     * cqc_destroy
     *
     * Free the resources allocated for this cqc_ctx handle (including itself).
     */
    void cqc_destroy(cqc_ctx *cqc)

    /*
     * send_cqc_header
     *
     * Prepare and send the top-level CQC header.
     *
     * Arguments:
     * type    the message type
     * len     length of subsequent headers
     */
    int send_cqc_header(cqc_ctx *cqc, uint8_t type, uint32_t len)

    /*
     * send_cqc_cmd
     *
     * Build and send the CQC header and the command header.
     *
     * Arguments:
     * command     command identifier to be sent
     * qubit_id    identifier of qubit on which to perform this command
     * notify      whether to request a DONE upon completion
     * action      set if there are more actions to execute when done
     * block       block other instructions until this command is done
     * length      length of any headers that are to follow
     */
    int send_cqc_cmd(cqc_ctx *cqc,
                     uint8_t command,
                     uint16_t qubit_id,
                     bool notify,
                     bool action,
                     bool block,
                     uint32_t length)

    /*
     * cqc_hello
     *
     * Sends a HELLO message to the CQC backend.
     */
    int cqc_hello(cqc_ctx *cqc)

    /*
     * cqc_simple_cmd
     *
     * Executes a simple CQC command (not requiring any additional details).
     *
     * Arguments:
     * command     command identifier to be sent
     * qubit_id    identifier of qubit on which to perform this command
     * notify      whether to request a DONE upon completion
     */
    int cqc_simple_cmd(cqc_ctx *cqc,
                       uint8_t command,
                       uint16_t qubit_id,
                       bool notify)

    /*
     * cqc_send
     *
     * Send a qubit to a remote node.
     *
     * Arguments:
     * qubit_id         qubit to send
     * remote_app_id    app id on the remote node to send to
     * remote_node      address of remote node (IPv4)
     * remote_port      port for classical control info
     */
    int cqc_send(cqc_ctx *cqc,
                 uint16_t qubit_id,
                 uint16_t remote_app_id,
                 uint16_t remote_port,
                 uint32_t remote_node)

    /*
     * cqc_recv
     *
     * Receive a qubit.
     *
     * Arguments:
     * qubit_id    (OUT) ID assigned to the received qubit
     */
    int cqc_recv(cqc_ctx *cqc, uint16_t *qubit_id)

    /*
     * cqc_measure
     *
     * Measure a specific qubit. This will block until the reply is received.
     *
     * Arguments:
     * qubit_id    qubit to measure
     * meas_out    (OUT) measurement outcome
     */
    int cqc_measure(cqc_ctx *cqc, uint16_t qubit_id, uint8_t *meas_out)

    /*
     * cqc_wait_until_done
     *
     * Receive certain number of DONE commands before proceeding.
     *
     * Arguments:
     * reps    number of replies to wait for
     */
    int cqc_wait_until_done(cqc_ctx *cqc, unsigned int reps)

    /*
     * cqc_wait_until_newok
     *
     * Wait until qubit creation is confirmed.
     *
     * Arguments:
     * qubit_id    (OUT) ID of created qubit
     */
    int cqc_wait_until_newok(cqc_ctx *cqc, uint16_t *qubit_id)

    /*
     *  cqc_twoqubit
     *
     *  Execute local two qubit gate.
     *
     *  Arguments:
     *  command    command id to execute
     *  qubit1     number of the first qubit
     *  qubit2     number of the second qubit
     */
    int cqc_twoqubit(cqc_ctx *cqc,
                     uint8_t command,
                     uint16_t qubit1,
                     uint16_t qubit2)

    /*
     * cqc_epr
     *
     * Generate EPR pair with remote node.
     *
     * Arguments:
     * remote_app_id    app id on the remote node
     * remote_node      address of remote node to receive from (IPv4)
     * remote_port      port for classical control info
     * qubit_id         (OUT) ID of local qubit of the EPR pair
     * ent_info         (OUT) etanglement information
     */
    int cqc_epr(cqc_ctx *cqc,
                uint16_t remote_app_id,
                uint16_t remote_port,
                uint32_t remote_node,
                uint16_t *qubit_id,
                entanglementHeader *ent_info)

    /*
     * cqc_epr_recv
     *
     * Receive EPR pair.
     *
     * qubit_id         (OUT) ID of local qubit of the EPR pair
     * ent_info         (OUT) etanglement information
     */
    int cqc_epr_recv(cqc_ctx *cqc,
                     uint16_t *qubit_id,
                     entanglementHeader *ent_info)
