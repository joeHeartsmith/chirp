![status:incomplete](https://img.shields.io/badge/status-incomplete-red)

# chirp - Composable Host Interior Routing Protocol
<img src="misc/logo.png" width="25%" height="25%">

An interior routing protocol designed to help solve and automate configuration tasks that are dependent on a host's location in a network topology.  For example, if you had 3 routers in a network and you needed to subnetwork an existing prefix (or DHCPv6 prefix delegation) between them, chirp would help the hosts converge and then each could independently calculate their subnetwork and update their routing tables accordingly.  This could also work for things like complex/NAT'ed tunnel setup/tear-down, easy construction/management of pure Layer-3 networks, or even generic orchestration tasks.  The point of this software is to act like a routing protocol, but then provide a scriptable interface to let users do extensible tasks once the topology is converged.

Goals
- [ ] Work with any protocol.  This was originally just supposed to automate the distribution of DHCPv6 PDs, but should work under any Layer 3 protocol
- [ ] Portability.  This should just be C99 code that uses the standard C library.  It should just work on anything from a SOHO router to a Windows host.
- [ ] Security.  A routing protocol should be secure.  A routing protocol that is expected to run arbitrary commands should be _extra_ secure.

