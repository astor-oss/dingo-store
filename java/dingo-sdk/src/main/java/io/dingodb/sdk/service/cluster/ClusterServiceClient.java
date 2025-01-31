/*
 * Copyright 2021 DataCanvas
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package io.dingodb.sdk.service.cluster;

import io.dingodb.common.Common;
import io.dingodb.coordinator.Coordinator;
import io.dingodb.coordinator.CoordinatorServiceGrpc;
import io.dingodb.sdk.common.cluster.Executor;
import io.dingodb.sdk.common.utils.EntityConversion;
import io.dingodb.sdk.service.connector.ServiceConnector;

import java.util.List;
import java.util.stream.Collectors;

import static io.dingodb.sdk.common.utils.EntityConversion.mapping;

public class ClusterServiceClient {

    private ServiceConnector<CoordinatorServiceGrpc.CoordinatorServiceBlockingStub> connector;

    public ClusterServiceClient(ServiceConnector<CoordinatorServiceGrpc.CoordinatorServiceBlockingStub> connector) {
        this.connector = connector;
    }

    public void executorHeartbeat(long epoch, Executor executor) {
        Coordinator.ExecutorHeartbeatRequest req = Coordinator.ExecutorHeartbeatRequest.newBuilder()
                .setSelfExecutormapEpoch(epoch)
                .setExecutor(mapping(executor))
                .build();

        Coordinator.ExecutorHeartbeatResponse response = connector.exec(stub -> stub.executorHeartbeat(req));
    }

    public List<Executor> getExecutorMap(long epoch) {
        Coordinator.GetExecutorMapRequest req = Coordinator.GetExecutorMapRequest.newBuilder()
                .setEpoch(epoch)
                .build();

        Coordinator.GetExecutorMapResponse response = connector.exec(stub -> stub.getExecutorMap(req));
        return response.getExecutormap()
                .getExecutorsList()
                .stream()
                .filter(executor -> executor.getState() == Common.ExecutorState.EXECUTOR_NORMAL)
                .map(EntityConversion::mapping)
                .collect(Collectors.toList());
    }
}
