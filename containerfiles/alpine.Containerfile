# SPDX-License-Identifier: LGPL-2.1-or-later

# # `bd_chapters` Containerfile
# Built on alpine (muslc) for it's insane size
#
# ## Builder
# Builder image
#
FROM docker.io/alpine AS builder
# ### Get apk package db
# This is important to do first and then delete later
# Most of my alpine Containerfiles look like this so
# a cache win with minimal dev overhead
#
RUN apk update
# ### Get install dev libs / tools
# Needs to be in the Containerfile, else it won't be cached
#
RUN apk add go-task g++ libbluray-dev
# ### Get code in
# Copy the base in and do switch
#
COPY . /source
WORKDIR /source
# ### Remove cache
# Delete cache from the container (remember to use squash)
#
RUN rm -rf /var/cache/apk/*
# ### Build
#
RUN go-task build



# ## Final image
# Based on alpine
#
FROM docker.io/alpine AS final
# ### Get apk package db
# See above.
#
RUN apk update
RUN apk add libbluray
RUN rm -rf /var/cache/apk/*
# ### Make alpine distroless-ish
# `¯\_(ツ)_/¯`
# IDK if this does what I want it to.
#
RUN rm -vrf /bin/* /sbin/* /usr/bin/* /usr/sbin/* /etc/busybox-paths.d /usr/share/apk /var/cache/apk /lib/apk /etc/apk
# ### Move from builder image
# Position is important for caching
COPY --from=builder /usr/local/bin/bd_chapters /usr/local/bin/bd_chapters
# ### Set container entrypoint and default cmd
#
ENTRYPOINT ["/usr/local/bin/bd_chapters"]
CMD ["-h"]
