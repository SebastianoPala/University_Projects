package com.unipi.PlayerHive.DTO.users;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.NotNull;
import jakarta.validation.constraints.Positive;
import jakarta.validation.constraints.PositiveOrZero;
import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

@AllArgsConstructor
@NoArgsConstructor
@Getter
@Setter

public class AddGameToLibraryDTO {
    @NotNull
    @NotBlank
    private String gameId;

    @NotNull
    @PositiveOrZero
    private Integer achievements;

    @NotNull
    @Positive
    private Float hoursPlayed;
}
