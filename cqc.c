#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#include "cqc.h"

/*
 * cqc_init
 *
 * Initialize the CQC Backend.
 *
 * Arguments:
 * app_id    ID to use for this application
 */
cqc_ctx *cqc_init(uint16_t app_id)
{
    /* Initialize CQC Data structure */
    cqc_ctx *cqc = (cqc_ctx *)malloc(sizeof(cqc_ctx));
    memset(cqc, 0x00, sizeof(cqc_ctx));
    cqc->app_id = app_id;

    return cqc;
}

/*
 * cqc_connect
 *
 * Connect to CQC Backend (if necessary).
 *
 * Arguments:
 * hostname     hostname to connect to
 * portno       port number to connect to
 */
int cqc_connect(cqc_ctx *cqc, char *hostname, int portno)
{
    int sock;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    /* Set up connection details */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("ERROR opening socket");
        return CQC_LIB_ERR;
    }

    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return CQC_LIB_ERR;
    }

    memset((char *) &serv_addr, 0x00, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memmove((char *)&serv_addr.sin_addr.s_addr,
            (char *)server->h_addr_list[0],
            server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Now connect to the server */
    if (connect(sock,
                (struct sockaddr*)&serv_addr,
                sizeof(serv_addr)) < 0) {
        perror("ERROR - Cannot connect");
        return CQC_LIB_ERR;
    }
    cqc->sockfd = sock;
    return CQC_LIB_OK;
}

/*
 * cqc_close
 *
 * Close the connection to the back-end.
 */
void cqc_close(cqc_ctx *cqc)
{
    close(cqc->sockfd);
}

/*
 * cqc_destroy
 *
 * Free allocated memory.
 */
void cqc_destroy(cqc_ctx *cqc)
{
    free(cqc);
}

/*
 * cqc_error
 *
 * Print the appropriate error message for the error code received.
 */
static void cqc_error(uint8_t type)
{
    switch(type) {
    case CQC_ERR_GENERAL:
        fprintf(stderr,"CQC ERROR: General error.\n");
        break;
    case CQC_ERR_NOQUBIT:
        fprintf(stderr,"CQC ERROR: No more qubits available.\n");
        break;
    case CQC_ERR_UNSUPP:
        fprintf(stderr,"CQC ERROR: Command not supported.\n");
        break;
    case CQC_ERR_TIMEOUT:
        fprintf(stderr,"CQC ERROR: Timeout.\n");
        break;
    case CQC_ERR_INUSE:
        fprintf(stderr,"CQC ERROR: Qubit already in use.\n");
        break;
    case CQC_ERR_UNKNOWN:
        fprintf(stderr,"CQC ERROR: Unknown qubit ID.");
        break;
    default:
        fprintf(stderr,"CQC ERROR: Unknown error type.\n");
    }
    return;
}

/*
 * send_cqc_header
 *
 * Prepare and sends CQC header
 */
static int send_cqc_header(cqc_ctx *cqc, uint8_t type, uint32_t len)
{
    cqcHeader cqcH;
    cqcH.version = CQC_VERSION;
    cqcH.type = type;
    cqcH.app_id = htons(cqc->app_id);
    cqcH.length = htonl(len);

    /* Send message to the server */
    int n = write(cqc->sockfd, &cqcH, CQC_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR writing to socket");
        return CQC_LIB_ERR;
    }

    return CQC_LIB_OK;
}

/*
 * send_cqc_cmd
 *
 * Build and send the CQC header and the command header
 *
 * Arguments:
 * command     command identifier to be sent
 * qubit_id    identifier of qubit on which to perform this command
 * notify      whether to request a DONE upon completion
 * length      length of any headers that are to follow
 */
static int send_cqc_cmd(cqc_ctx *cqc,
                        uint8_t command,
                        uint16_t qubit_id,
                        bool notify,
                        bool action,
                        bool block,
                        uint32_t length)
{
    /* Send CQC message indicating a command */
    int rc = send_cqc_header(cqc, CQC_TP_COMMAND, CQC_CMD_HDR_LENGTH + length);
    if (rc != CQC_LIB_OK) {
        fprintf(stderr, "Failed to send CQC header.\n");
        return CQC_LIB_ERR;
    }

    /* Prepare message for the specific command */
    cmdHeader cmdH;
    memset(&cmdH, 0x00, sizeof(cmdH));
    cmdH.qubit_id = htons(qubit_id);
    cmdH.instr = command;
    if (notify) {
        cmdH.options = cmdH.options | CQC_OPT_NOTIFY;
    }
    if (action) {
        cmdH.options = cmdH.options | CQC_OPT_ACTION;
    }
    if (block) {
        cmdH.options = cmdH.options | CQC_OPT_BLOCK;
    }

    /* Send message to the server */
    int n = write(cqc->sockfd, &cmdH, CQC_CMD_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR writing to socket");
        return CQC_LIB_ERR;
    }

    return CQC_LIB_OK;
}

/*
 * cqc_hello
 *
 * Sends a HELLO message to the CQC Backend.
 */
int cqc_hello(cqc_ctx *cqc)
{
    return send_cqc_header(cqc, CQC_TP_HELLO, 0);
}

/*
 * cqc_simple_cmd
 *
 * Executes a simple CQC command (not requiring any additional details)
 *
 * Arguments:
 * command     command identifier to be sent
 * qubit_id    identifier of qubit on which to perform this command
 * notify      whether to request a DONE upon completion (0 = no, 1 = yes)
 */
int cqc_simple_cmd(cqc_ctx *cqc,
                   uint8_t command,
                   uint16_t qubit_id,
                   bool notify)
{
    return send_cqc_cmd(cqc, command, qubit_id, notify, false, notify, 0);
}

/*
 * cqc_send
 *
 * Request the qubit to send to remote node.
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
{
    int rc = send_cqc_cmd(cqc,
                          CQC_CMD_SEND,
                          qubit_id,
                          true,
                          false,
                          false,
                          CQC_COMM_HDR_LENGTH);
    if (rc != CQC_LIB_OK) {
        fprintf(stderr, "Failed to send CQC command.\n");
        return CQC_LIB_ERR;
    }

    /* Prepare message for the specific command */
    commHeader commH;
    memset(&commH, 0x00, sizeof(commH));
    commH.remote_app_id = htons(remote_app_id);
    commH.remote_port = htons(remote_port);
    commH.remote_node = htonl(remote_node);

    /* Send message to the server */
    int n = write(cqc->sockfd, &commH, CQC_COMM_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR writing to socket");
        return CQC_LIB_ERR;
    }

    return CQC_LIB_OK;
}

/*
 * cqc_recv
 *
 * Request to receive a qubit.
 *
 * Arguments:
 * qubit_id    id to assign to this qubit once it is received
 */
int cqc_recv(cqc_ctx *cqc, uint16_t *qubit_id)
{
    int n;
    cqcHeader reply;
    qubitHeader note;

    /* Send out request to receive a qubit */
    n = cqc_simple_cmd(cqc, CQC_CMD_RECV, 0, 0);
    if (n < 0) {
        perror("ERROR - Cannot send receive request");
        return CQC_LIB_ERR;
    }

    /* Now read CQC Header from server response */
    memset(&reply, 0x00, sizeof(reply));
    n = read(cqc->sockfd, &reply, CQC_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR - cannot get reply header");
        return CQC_LIB_ERR;
    }
    if(reply.type != CQC_TP_RECV) {
        fprintf(stderr,"ERROR: Expected RECV");
        return CQC_LIB_ERR;
    }

    /* Then read qubit id from notify header */
    memset(&note, 0x00, sizeof(note));
    n = read(cqc->sockfd, &note, CQC_QUBIT_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR - cannot get measurement outcome");
        return CQC_LIB_ERR;
    }

    *qubit_id = ntohs(note.qubit_id);
    return CQC_LIB_OK;
}

/*
 * cqc_measure
 *
 * Request to measure a specific qubit. This will block until the reply is
 * received. (Non blocking measure requests can be performed using
 * cqc_simple_cmd)
 *
 * Arguments:
 * qubit_id    qubit to measure
 */
int cqc_measure(cqc_ctx *cqc, uint16_t qubit_id, uint8_t *meas_out)
{
    int n;
    assignHeader assign;
    cqcHeader reply;
    measoutHeader note;

    /* Send command to perform measurement */
    send_cqc_cmd(cqc,
                 CQC_CMD_MEASURE,
                 qubit_id,
                 false,
                 false,
                 false,
                 CQC_ASSIGN_HDR_LENGTH);

    /* Send the assign header */
    memset(&assign, 0x00, sizeof(assign));
    n = write(cqc->sockfd, &assign, CQC_ASSIGN_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR writing to socket");
        return CQC_LIB_ERR;
    }

    /* Now read CQC Header from server response */
    memset(&reply, 0x00, sizeof(reply));
    n = read(cqc->sockfd, &reply, CQC_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR - cannot get reply header");
        return CQC_LIB_ERR;
    }
    if(reply.type != CQC_TP_MEASOUT) {
        fprintf(stderr,"ERROR: Expected MEASOUT, got %u\n",reply.type);
        return CQC_LIB_ERR;
    }

    /* Then read measurement outcome from notify header */
    memset(&note, 0x00, sizeof(note));
    n = read(cqc->sockfd, &note, CQC_MEASOUT_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR - cannot get measurement outcome");
        return CQC_LIB_ERR;
    }

    *meas_out = note.meas_out;
    return CQC_LIB_OK;
}

/*
 * cqc_wait_until_done
 *
 * Read a certain number of DONE commands before proceeding.
 *
 * Arguments:
 * reps    number of replies to wait for
 */
int cqc_wait_until_done(cqc_ctx *cqc, unsigned int reps)
{
    int i, n;
    cqcHeader reply;

    for(i = 0; i < reps; i++) {
        /* Now read CQC Header from server response */
        memset(&reply, 0x00, sizeof(reply));
        n = read(cqc->sockfd, &reply, CQC_HDR_LENGTH);
        if (n < 0) {
            perror("ERROR - cannot get reply header");
            return CQC_LIB_ERR;
        }

        /* Check whether an error occured */
        if(reply.type >= CQC_ERR_GENERAL) {
            cqc_error(reply.type);
            return CQC_LIB_ERR;
        }

        /* Otherwise check whether it's done */
        if(reply.type != CQC_TP_DONE) {
            fprintf(stderr,"Unexpected reply of type %d\n",reply.type);
            return CQC_LIB_ERR;
        }
    }

    return CQC_LIB_OK;
}


/*
 * cqc_wait_until_newok
 *
 * Wait until qubit creation is confirmed. Returns qubit id if successful, -1 otherwise.
 *
 */
int cqc_wait_until_newok(cqc_ctx *cqc, uint16_t *qubit_id)
{
    int n;
    cqcHeader reply;
    qubitHeader note;

    /* Now read CQC Header from server response */
    memset(&reply, 0x00, sizeof(reply));
    n = read(cqc->sockfd, &reply, CQC_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR - cannot get reply header");
        return CQC_LIB_ERR;
    }

    /* Check whether an error occured */
    if(reply.type >= CQC_ERR_GENERAL) {
        cqc_error(reply.type);
        return CQC_LIB_ERR;
    }

    /* Otherwise check whether it's done */
    if(reply.type != CQC_TP_NEW_OK) {
        fprintf(stderr,"Unexpected reply of type %d, expected %d\n",reply.type, CQC_TP_NEW_OK);
        return CQC_LIB_ERR;
    }

    /* Then read qubit id from notify header */
    memset(&note, 0x00, sizeof(note));
    n = read(cqc->sockfd, &note, CQC_QUBIT_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR - cannot get measurement outcome");
        return CQC_LIB_ERR;
    }

    *qubit_id = ntohs(note.qubit_id);
    return CQC_LIB_OK;
}

/*
 *  cqc_twoqubit
 *
 *  Execute local two qubit gate.
 *
 *  Arguments:
 *  command     command id to execute
 *  qubit1      number of the first qubit
 *  qubit2      number of the second qubit
 */
int cqc_twoqubit(cqc_ctx *cqc,
                 uint8_t command,
                 uint16_t qubit1,
                 uint16_t qubit2)
{
    int rc = send_cqc_cmd(cqc,
                          command,
                          qubit1,
                          false,
                          false,
                          true,
                          CQC_QUBIT_HDR_LENGTH);
    if (rc != CQC_LIB_OK) {
        fprintf(stderr, "Failed to send CQC command.\n");
        return CQC_LIB_ERR;
    }

    /* Prepare message for the specific command */
    qubitHeader qubitH;
    memset(&qubitH, 0x00, sizeof(qubitH));
    qubitH.qubit_id = htons(qubit2);

    /* Send message to the server */
    int n = write(cqc->sockfd, &qubitH, CQC_QUBIT_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR writing to socket");
        return CQC_LIB_ERR;
    }

    return CQC_LIB_OK;
}

/*
 * cqc_ntohll
 *
 * Convert 64-bit value to host order. Linux needs endian.h, but cannot use
 * endian.h on a mac so this is the portable solution.
 *
 */

static uint64_t cqc_ntohll(const uint64_t input)
{
    uint8_t val[8];

    val[0] = input >> 56;
    val[1] = input >> 48;
    val[2] = input >> 40;
    val[3] = input >> 32;
    val[4] = input >> 24;
    val[5] = input >> 16;
    val[6] = input >> 8;
    val[7] = input >> 0;

    return *(uint64_t *)val;
}

/*
 * cqc_ntoh_epr_hdr
 *
 * Convert EPR header from network to host order.
 */
static void cqc_ntoh_epr_hdr(entanglementHeader *ent_info)
{
    ent_info->node_A = ntohl(ent_info->node_A);
    ent_info->port_A = ntohs(ent_info->port_A);
    ent_info->app_id_A = ntohs(ent_info->app_id_A);
    ent_info->node_B = ntohl(ent_info->node_B);
    ent_info->port_B = ntohs(ent_info->port_B);
    ent_info->app_id_B = ntohs(ent_info->app_id_B);
    ent_info->id_AB = ntohl(ent_info->id_AB);
    ent_info->timestamp = cqc_ntohll(ent_info->timestamp);
    ent_info->tog = cqc_ntohll(ent_info->tog);
    ent_info->goodness = ntohs(ent_info->goodness);
}

/*
 * cqc_epr
 *
 * Request to generate EPR pair with remote node.
 *
 * Arguments:
 * remote_app_id    app id on the remote node to send to
 * remote_node      address of remote node to receive from (IPv4)
 * remote_port      port for classical control info
 */
int cqc_epr(cqc_ctx *cqc,
            uint16_t remote_app_id,
            uint16_t remote_port,
            uint32_t remote_node,
            uint16_t *qubit_id,
            entanglementHeader *ent_info)
{
    int rc = send_cqc_cmd(cqc,
                          CQC_CMD_EPR,
                          0,
                          true,
                          false,
                          true,
                          CQC_COMM_HDR_LENGTH);
    if (rc != CQC_LIB_OK) {
        fprintf(stderr, "Failed to send CQC command.\n");
        return CQC_LIB_ERR;
    }

    /* Prepare message for the specific command */
    commHeader commH;
    memset(&commH, 0x00, sizeof(commH));
    commH.remote_app_id = htons(remote_app_id);
    commH.remote_port = htons(remote_port);
    commH.remote_node = htonl(remote_node);

    /* Send message to the server */
    int n = write(cqc->sockfd, &commH, CQC_COMM_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR writing to socket");
        return CQC_LIB_ERR;
    }

    /* Now read CQC Header from server response */
    cqcHeader reply;
    memset(&reply, 0x00, sizeof(reply));
    n = read(cqc->sockfd, &reply, CQC_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR - cannot get reply header");
        return CQC_LIB_ERR;
    }
    if (reply.type != CQC_TP_MEASOUT) {
        fprintf(stderr, "ERROR: Expected MEASOUT, got %u\n", reply.type);
        return CQC_LIB_ERR;
    }

    /* Then read qubit id from notify header */
    qubitHeader note;
    memset(&note, 0x00, sizeof(note));
    n = read(cqc->sockfd, &note, CQC_QUBIT_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR - cannot get measurement outcome");
        return CQC_LIB_ERR;
    }
    *qubit_id = ntohs(note.qubit_id);

    /* Read the entanglement header */
    memset(ent_info, 0x00, sizeof(*ent_info));
    n = read(cqc->sockfd, ent_info, CQC_ENT_INFO_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR - cannot get entanglement  info header");
        return CQC_LIB_ERR;
    }
    cqc_ntoh_epr_hdr(ent_info);

    return CQC_LIB_OK;
}

/*
 * cqc_epr_recv
 *
 * Request to receive EPR pair.
 *
 */
int cqc_epr_recv(cqc_ctx *cqc,
                 uint16_t *qubit_id,
                 entanglementHeader *ent_info)
{
    int rc = send_cqc_cmd(cqc,
                          CQC_CMD_EPR_RECV,
                          0,
                          true,
                          false,
                          true,
                          CQC_COMM_HDR_LENGTH);

    if (rc != CQC_LIB_OK) {
        fprintf(stderr, "Failed to send CQC command.\n");
        return CQC_LIB_ERR;
    }

    /* Now read CQC Header from server response */
    cqcHeader reply;
    memset(&reply, 0x00, sizeof(reply));
    int n = read(cqc->sockfd, &reply, CQC_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR - cannot get reply header");
        return CQC_LIB_ERR;
    }
    if (reply.type != CQC_TP_MEASOUT) {
        fprintf(stderr, "ERROR: Expected MEASOUT, got %u\n", reply.type);
        return CQC_LIB_ERR;
    }

    /* Then read qubit id from notify header */
    qubitHeader note;
    memset(&note, 0x00, sizeof(note));
    n = read(cqc->sockfd, &note, CQC_QUBIT_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR - cannot get measurement outcome");
        return CQC_LIB_ERR;
    }
    *qubit_id = ntohs(note.qubit_id);

    /* Read the entanglement header */
    memset(ent_info, 0x00, sizeof(*ent_info));
    n = read(cqc->sockfd, ent_info, CQC_ENT_INFO_HDR_LENGTH);
    if (n < 0) {
        perror("ERROR - cannot get entanglement  info header");
        return CQC_LIB_ERR;
    }
    cqc_ntoh_epr_hdr(ent_info);

    return CQC_LIB_OK;
}
