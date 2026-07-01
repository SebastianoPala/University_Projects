package com.unipi.PlayerHive.DTO.games;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.NotNull;
import jakarta.validation.constraints.PastOrPresent;
import jakarta.validation.constraints.PositiveOrZero;
import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

import java.time.LocalDate;
import java.util.List;

@NoArgsConstructor
@AllArgsConstructor
@Getter
@Setter

public class AddGameDTO {

    @NotNull
    @NotBlank
    private String name;

    @NotNull
    @PastOrPresent
    private LocalDate releaseDate;

    @NotNull
    @PositiveOrZero
    private Double price;

    @NotNull
    @PositiveOrZero
    private Integer discount;

    @NotNull
    private String description;

    @NotNull
    @NotBlank
    private String imageURL;

    @NotNull
    private List<String> supportedOS;

    @NotNull
    @PositiveOrZero
    private Integer achievements;

    @NotNull
    private List<String> developers;

    @NotNull
    private List<String> publishers;

    @NotNull
    private List<String> genres;
}
