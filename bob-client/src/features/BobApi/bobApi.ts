import type { Action } from '@reduxjs/toolkit';
import { createApi, fetchBaseQuery } from '@reduxjs/toolkit/query/react';
import type {
    BaseQueryFn,
    FetchArgs,
    FetchBaseQueryError,
} from '@reduxjs/toolkit/query/react';
import { REHYDRATE } from 'redux-persist';

import type { RootState } from 'app/store';
import { selectBobIp } from 'features/Bob/bobSlice';

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

const rawBaseQuery = fetchBaseQuery({
    baseUrl: '',
});

const dynamicBaseQuery: BaseQueryFn<
    string | FetchArgs,
    unknown,
    FetchBaseQueryError
> = async (args, api, extraOptions) => {
    const ip = selectBobIp(api.getState() as RootState);
    if (!ip) {
        return {
            error: {
                status: 400,
                statusText: 'Bad Request',
                data: 'No IP address provided',
            },
        };
    }

    const urlEnd = typeof args === 'string' ? args : args.url;
    const adjustedUrl = `http://${ip}/${urlEnd}`;
    const adjustedArgs =
        typeof args === 'string' ? adjustedUrl : { ...args, url: adjustedUrl };

    return rawBaseQuery(adjustedArgs, api, extraOptions);
};

/**
 * Converts an Int16Array to a WAV file blob
 * @param audioData - The audio data as Int16Array
 * @param sampleRate - The sample rate of the audio (default: 24000)
 * @returns Blob - WAV file blob
 */
const convertToWav = (audioData: Int16Array, sampleRate = 24000): Blob => {
    const buffer = new ArrayBuffer(44 + audioData.length * 2);
    const view = new DataView(buffer);

    // Write WAV header
    // "RIFF" identifier
    view.setUint8(0, 0x52); // R
    view.setUint8(1, 0x49); // I
    view.setUint8(2, 0x46); // F
    view.setUint8(3, 0x46); // F

    // File length minus RIFF identifier length and file description length
    view.setUint32(4, 36 + audioData.length * 2, true);

    // "WAVE" identifier
    view.setUint8(8, 0x57); // W
    view.setUint8(9, 0x41); // A
    view.setUint8(10, 0x56); // V
    view.setUint8(11, 0x45); // E

    // "fmt " chunk identifier
    view.setUint8(12, 0x66); // f
    view.setUint8(13, 0x6d); // m
    view.setUint8(14, 0x74); // t
    view.setUint8(15, 0x20); // " "

    // Chunk length
    view.setUint32(16, 16, true);
    // Sample format (raw)
    view.setUint16(20, 1, true);
    // Channel count
    view.setUint16(22, 1, true);
    // Sample rate
    view.setUint32(24, sampleRate, true);
    // Byte rate (sample rate * block align)
    view.setUint32(28, sampleRate * 2, true);
    // Block align (channel count * bytes per sample)
    view.setUint16(32, 2, true);
    // Bits per sample
    view.setUint16(34, 16, true);

    // "data" chunk identifier
    view.setUint8(36, 0x64); // d
    view.setUint8(37, 0x61); // a
    view.setUint8(38, 0x74); // t
    view.setUint8(39, 0x61); // a

    // Chunk length
    view.setUint32(40, audioData.length * 2, true);

    // Write audio data
    for (let i = 0; i < audioData.length; i++) {
        view.setInt16(44 + i * 2, audioData[i], true);
    }

    return new Blob([buffer], { type: 'audio/wav' });
};

export const bobApi = createApi({
    reducerPath: 'bobApi',
    baseQuery: dynamicBaseQuery,
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
    extractRehydrationInfo(action): any {
        if (isHydrateAction(action)) {
            // When persisting the api reducer
            if (action.key === 'key used with redux-persist') {
                return action.payload;
            }

            // When persisting the root reducer
            return action.payload[bobApi.reducerPath];
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
