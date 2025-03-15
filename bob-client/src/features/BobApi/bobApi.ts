import type { Action } from '@reduxjs/toolkit';
import { createApi, fetchBaseQuery } from '@reduxjs/toolkit/query/react';
import { REHYDRATE } from 'redux-persist';

import { convertToWav } from './convertToWav';

import type { RootState } from 'app/store';

function isHydrateAction(action: Action): action is Action<typeof REHYDRATE> & {
    key: string;
    payload: RootState;
    err: unknown;
} {
    return action.type === REHYDRATE;
}

export type HealthcheckResponse = {
    status: string;
    apiKey: string;
};

export type AudioUploadResponse = {
    status: string;
    size: number;
};

export type MoveCommandRequest = {
    type: 'reset' | 'standUp' | 'wiggle' | 'sitDown';
};

export type MoveCommandResponse = {
    status: string;
};

export const bobApi = createApi({
    reducerPath: 'bobApi',
    baseQuery: fetchBaseQuery({
        baseUrl: '/bob',
    }),
    endpoints: (builder) => ({
        /**
         * Health check query.
         */
        healthcheck: builder.query<HealthcheckResponse, void>({
            query: () => `health-check`,
        }),
        /**
         * Capture photo query.
         */
        capture: builder.query<string, void>({
            query: () => ({
                url: 'capture',
                responseHandler: (response) => response.blob(),
            }),
            transformResponse: (blob: Blob) => URL.createObjectURL(blob),
            forceRefetch: () => true,
        }),
        /**
         * Upload and play audio mutation.
         * @param audioData - Int16Array containing the audio data
         */
        uploadAudio: builder.mutation<AudioUploadResponse, Int16Array>({
            query: (audioData) => {
                // Convert the Int16Array to WAV format
                const wavBlob = convertToWav(audioData);

                // Create FormData and append the WAV file
                const formData = new FormData();
                formData.append('file', wavBlob, 'audio.wav');

                return {
                    url: 'audio',
                    method: 'POST',
                    body: formData,
                    // Don't set Content-Type header - browser will set it with boundary for multipart/form-data
                    formData: true,
                };
            },
        }),
        /**
         * Stop audio mutation.
         */
        stopAudio: builder.mutation<{ status: string }, void>({
            query: () => ({
                url: 'stop-audio',
                method: 'POST',
            }),
        }),
        /**
         * Move command mutation.
         * @param command - Object containing the movement type (reset, standUp, wiggle)
         */
        moveCommand: builder.mutation<MoveCommandResponse, MoveCommandRequest>({
            query: (command) => ({
                url: 'move',
                method: 'POST',
                body: command,
            }),
        }),
    }),
    // Official documentation states we have to use "any"
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    extractRehydrationInfo(action, { reducerPath }): any {
        if (isHydrateAction(action)) {
            // When persisting the api reducer
            if (action.key === 'key used with redux-persist') {
                return action.payload;
            }
            // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
            if (!action.payload) {
                return undefined;
            }
            // When persisting the root reducer
            return action.payload[reducerPath];
        }
    },
});

export const {
    useLazyHealthcheckQuery,
    useUploadAudioMutation,
    useLazyCaptureQuery,
    useStopAudioMutation,
    useMoveCommandMutation,
} = bobApi;

export const useHealthcheckQueryState =
    bobApi.endpoints.healthcheck.useQueryState;
